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
#include "domain_json_util.h"
#include "bundle_info_query.h"
#include "app_domain_verify_task_mgr.h"
#include "iservice_registry.h"
#include "app_domain_verify_mgr_client.h"

namespace OHOS {
namespace AppDomainVerify {
const std::string HTTPS = "https";
const std::set<std::string> SCHEME_WHITE_SET = { HTTPS };
const std::string FUZZY_HOST_START = "*.";
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(new AppDomainVerifyAgentService());
constexpr int32_t UNLOAD_IMMEDIATELY = 0;
constexpr int32_t UNLOAD_DELAY_TIME = 120000;  // 2min
const std::string BOOT_COMPLETED_EVENT = "usual.event.BOOT_COMPLETED";
const std::string LOOP_EVENT = "loopevent";
AppDomainVerifyAgentService::AppDomainVerifyAgentService()
    : SystemAbility(APP_DOMAIN_VERIFY_AGENT_SA_ID, true)
{
    appDomainVerifyExtMgr_ = std::make_shared<AppDomainVerifyExtensionMgr>();
    appDomainVerifyTaskMgr_ = std::make_unique<AppDomainVerifyTaskMgr>();

    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "new instance create.");
}
AppDomainVerifyAgentService::~AppDomainVerifyAgentService()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "instance dead.");
}
void AppDomainVerifyAgentService::CompleteVerifyRefresh(const BundleVerifyStatusInfo &bundleVerifyStatusInfo,
    const std::vector<InnerVerifyStatus> &statuses, int delaySeconds)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND !=
        appDomainVerifyExtMgr_->CompleteVerifyRefresh(bundleVerifyStatusInfo, statuses, delaySeconds)) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
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
        SingleVerify(appVerifyBaseInfo, skillUris);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}

void AppDomainVerifyAgentService::SingleVerify(const AppVerifyBaseInfo &appVerifyBaseInfo,
    const std::vector<SkillUri> &skillUris)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND != appDomainVerifyExtMgr_->SingleVerify(appVerifyBaseInfo, skillUris)) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
    }
    std::unordered_map<std::string, InnerVerifyStatus> uriVerifyMap;
    InitUriVerifyMap(skillUris, uriVerifyMap);
    if (uriVerifyMap.empty()) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "no valid skillUris");
        return;
    }
    auto task = std::make_shared<Task>(TaskType::IMMEDIATE_TASK, appVerifyBaseInfo, uriVerifyMap);
    if (appDomainVerifyTaskMgr_ != nullptr) {
        appDomainVerifyTaskMgr_->AddTask(task);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}

void AppDomainVerifyAgentService::InitUriVerifyMap(const std::vector<SkillUri> &skillUris,
    std::unordered_map<std::string, InnerVerifyStatus> &uriVerifyMap)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    for (auto it = skillUris.begin(); it != skillUris.end(); ++it) {
        if (it->scheme.empty() || it->host.empty() || !UrlUtil::IsValidAppDomainVerifyHost(it->host) ||
            SCHEME_WHITE_SET.find(it->scheme) == SCHEME_WHITE_SET.end()) {
            APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "invalid skillUri skip.");
            continue;
        }

        std::string host = it->host;
        if (it->host.substr(0, FUZZY_HOST_START.size()) == FUZZY_HOST_START) {
            // Hosts with *.
            host = it->host.substr(FUZZY_HOST_START.size());
        }
        // validUris remove duplicates
        uriVerifyMap.insert(make_pair(it->scheme + "://" + host, InnerVerifyStatus::UNKNOWN));
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}

void AppDomainVerifyAgentService::QueryAndCompleteRefresh(const std::vector<InnerVerifyStatus> &statuses,
    int delaySeconds)
{
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    if (AppDomainVerifyMgrClient::GetInstance()->QueryAllDomainVerifyStatus(bundleVerifyStatusInfo)) {
        CompleteVerifyRefresh(bundleVerifyStatusInfo, statuses, delaySeconds);
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
        auto func = [this]() {
            QueryAndCompleteRefresh(
                std::vector<InnerVerifyStatus>{ InnerVerifyStatus::UNKNOWN, InnerVerifyStatus::STATE_FAIL,
                    InnerVerifyStatus::FAILURE_REDIRECT, InnerVerifyStatus::FAILURE_CLIENT_ERROR,
                    InnerVerifyStatus::FAILURE_REJECTED_BY_SERVER, InnerVerifyStatus::FAILURE_HTTP_UNKNOWN,
                    InnerVerifyStatus::FAILURE_TIMEOUT },
                0);
        };
        continuationHandler_->submit(func);
    }
    bool res = Publish(this);
    if (!res) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "Publish failed");
    }
    AppDomainVerifyAgentServiceStub::PostDelayUnloadTask();
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
