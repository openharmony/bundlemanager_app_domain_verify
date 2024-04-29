/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <thread>

#include "app_domain_verify_agent_service.h"
#include "iremote_broker.h"
#include "system_ability_definition.h"
#include "domain_url_util.h"
#include "app_domain_verify_hilog.h"
#include "bms/bundle_info_query.h"
#include "app_domain_verify_task_mgr.h"
#include "iservice_registry.h"
#include "app_domain_verify_mgr_client.h"
#include "app_domain_verify_hisysevent.h"
#include "verify_task.h"

namespace OHOS {
namespace AppDomainVerify {

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(new AppDomainVerifyAgentService());
constexpr int32_t UNLOAD_IMMEDIATELY = 0;
constexpr int32_t UNLOAD_DELAY_TIME = 120000;  // 2min
const std::string BOOT_COMPLETED_EVENT = "usual.event.BOOT_COMPLETED";
const std::string LOOP_EVENT = "loopevent";
AppDomainVerifyAgentService::AppDomainVerifyAgentService()
    : SystemAbility(APP_DOMAIN_VERIFY_AGENT_SA_ID, true)
{
    appDomainVerifyExtMgr_ = std::make_shared<AppDomainVerifyExtensionMgr>();
    appDomainVerifyTaskMgr_ = AppDomainVerifyTaskMgr::GetInstance();

    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "new instance create %p.",
        appDomainVerifyTaskMgr_.get());
}
AppDomainVerifyAgentService::~AppDomainVerifyAgentService()
{
    AppDomainVerifyTaskMgr::DestroyInstance();
    if (appDomainVerifyTaskMgr_ != nullptr) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "appDomainVerifyTaskMgr_ reset.");
        appDomainVerifyTaskMgr_ = nullptr;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "instance dead.");
}
void AppDomainVerifyAgentService::CompleteVerifyRefresh(const BundleVerifyStatusInfo &bundleVerifyStatusInfo,
    const std::vector<InnerVerifyStatus> &statuses, int delaySeconds, TaskType type)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND !=
        appDomainVerifyExtMgr_->CompleteVerifyRefresh(bundleVerifyStatusInfo, statuses, delaySeconds, type)) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
    }
    if (delaySeconds > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
    }
    for (auto it = bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.begin();
         it != bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.end(); it++) {
        std::vector<SkillUri> skillUris;
        for (auto itr = it->second.hostVerifyStatusMap.begin(); itr != it->second.hostVerifyStatusMap.end(); itr++) {
            if (std::find(statuses.begin(), statuses.end(), itr->second) != statuses.end()) {
                SkillUri skillUri;
                skillUri.host = UrlUtil::GetHost(itr->first);
                skillUri.scheme = UrlUtil::GetScheme(itr->first);
                skillUris.emplace_back(skillUri);
            }
        }
        AppVerifyBaseInfo appVerifyBaseInfo;
        appVerifyBaseInfo.bundleName = it->first;
        if (!BundleInfoQuery::GetBundleInfo(appVerifyBaseInfo.bundleName, appVerifyBaseInfo.appIdentifier,
            appVerifyBaseInfo.fingerprint)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "GetBundleInfo failed.");
            // todo delete this bundleName or not
            continue;
        }
        AddVerifyTask(appVerifyBaseInfo, skillUris, type);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}

void AppDomainVerifyAgentService::SingleVerify(const AppVerifyBaseInfo &appVerifyBaseInfo,
    const std::vector<SkillUri> &skillUris)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    AddVerifyTask(appVerifyBaseInfo, skillUris, TaskType::IMMEDIATE_TASK);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}

void AppDomainVerifyAgentService::AddVerifyTask(const AppVerifyBaseInfo &appVerifyBaseInfo,
    const std::vector<SkillUri> &skillUris, TaskType type)
{
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND != appDomainVerifyExtMgr_->SingleVerify(appVerifyBaseInfo, skillUris)) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
    }

    auto task = std::make_shared<VerifyTask>(type, appVerifyBaseInfo, skillUris);
    if (task->GetUriVerifyMap().empty()) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "no valid skillUris");
        return;
    }
    if (appDomainVerifyTaskMgr_ != nullptr) {
        appDomainVerifyTaskMgr_->AddTask(task);
    }
}

void AppDomainVerifyAgentService::QueryAndCompleteRefresh(const std::vector<InnerVerifyStatus> &statuses,
    int delaySeconds, TaskType type)
{
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    if (AppDomainVerifyMgrClient::GetInstance()->QueryAllDomainVerifyStatus(bundleVerifyStatusInfo)) {
        CompleteVerifyRefresh(bundleVerifyStatusInfo, statuses, delaySeconds, type);
    }
}

// sa_main进程统一调用
void AppDomainVerifyAgentService::OnStart(const SystemAbilityOnDemandReason &startReason)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnStart reason %{public}s, reasonId_:%{public}d",
        startReason.GetName().c_str(), startReason.GetId());
    if (continuationHandler_ == nullptr) {
        continuationHandler_ = std::make_shared<ffrt::queue>("VerifyServiceContinuationMgr");
    }

    if (startReason.GetName() == BOOT_COMPLETED_EVENT || startReason.GetName() == LOOP_EVENT) {
        // todo only for oobe
        TaskType type = startReason.GetName() == BOOT_COMPLETED_EVENT ?
            TaskType::BOOT_REFRESH_TASK :
            TaskType::SCHEDULE_REFRESH_TASK;
        auto func = [this, type]() {
            QueryAndCompleteRefresh(
                std::vector<InnerVerifyStatus>{ InnerVerifyStatus::UNKNOWN, InnerVerifyStatus::STATE_FAIL,
                    InnerVerifyStatus::FAILURE_REDIRECT, InnerVerifyStatus::FAILURE_CLIENT_ERROR,
                    InnerVerifyStatus::FAILURE_REJECTED_BY_SERVER, InnerVerifyStatus::FAILURE_HTTP_UNKNOWN,
                    InnerVerifyStatus::FAILURE_TIMEOUT },
                0, type);
        };
        continuationHandler_->submit(func);
    }
    
    AppDomainVerifyAgentServiceStub::PostDelayUnloadTask();
    bool res = Publish(this);
    if (!res) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "Publish failed");
    }
}

void AppDomainVerifyAgentService::OnStop()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
}

int32_t AppDomainVerifyAgentService::OnIdle(const SystemAbilityOnDemandReason &idleReason)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnIdle reason:%{public}s",
        idleReason.GetName().c_str());
    if (IsIdle()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnIdle unload immediately");
        return UNLOAD_IMMEDIATELY;
    } else {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnIdle delay unload");
        return UNLOAD_DELAY_TIME;
    }
}

void AppDomainVerifyAgentService::ExitIdleState()
{
    CancelIdle();
}

bool AppDomainVerifyAgentService::IsIdle()
{
    if (appDomainVerifyTaskMgr_ == nullptr) {
        return true;
    } else {
        return appDomainVerifyTaskMgr_->IsIdle();
    }
}
}  // namespace AppDomainVerify
}  // namespace OHOS
