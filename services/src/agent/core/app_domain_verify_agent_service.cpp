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

#include <memory>
#include <string>
#include <vector>
#include <thread>

#include "app_domain_verify_agent_service.h"
#include "system_ability_definition.h"
#include "domain_url_util.h"
#include "bms/bundle_info_query.h"
#include "app_domain_verify_mgr_client.h"
#include "verify_task.h"
#include "setting_data_share_helper.h"
#include "os_account_manager.h"

namespace OHOS {
namespace AppDomainVerify {

static std::atomic<bool> needCheckOOBE = false;
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(new AppDomainVerifyAgentService());
constexpr int32_t UNLOAD_IMMEDIATELY = 0;
constexpr int32_t UNLOAD_DELAY_TIME = 120000;  // 2min
const std::string BOOT_COMPLETED_EVENT = "usual.event.BOOT_COMPLETED";
const std::string LOOP_EVENT = "loopevent";
AppDomainVerifyAgentService::AppDomainVerifyAgentService() : SystemAbility(APP_DOMAIN_VERIFY_AGENT_SA_ID, true)
{
    appDomainVerifyExtMgr_ = std::make_shared<AppDomainVerifyExtensionMgr>();
    appDomainVerifyTaskMgr_ = AppDomainVerifyTaskMgr::GetInstance();

    APP_DOMAIN_VERIFY_HILOGD(
        APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "new instance create %p.", appDomainVerifyTaskMgr_.get());
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
void AppDomainVerifyAgentService::CompleteVerifyRefresh(const BundleVerifyStatusInfo& bundleVerifyStatusInfo,
    const std::vector<InnerVerifyStatus>& statuses, int delaySeconds, TaskType type)
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
        if (!BundleInfoQuery::GetBundleInfo(
                appVerifyBaseInfo.bundleName, appVerifyBaseInfo.appIdentifier, appVerifyBaseInfo.fingerprint)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "GetBundleInfo failed.");
            // todo delete this bundleName or not
            continue;
        }
        ExecuteVerifyTask(appVerifyBaseInfo, skillUris, type);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}
void AppDomainVerifyAgentService::ConvertToExplicitWant(
    OHOS::AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND !=
        appDomainVerifyExtMgr_->ConvertToExplicitWant(implicitWant, callback)) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
    }

    if (callback) {
        callback->OnConvert(0, implicitWant);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
}
void AppDomainVerifyAgentService::SingleVerify(
    const AppVerifyBaseInfo& appVerifyBaseInfo, const std::vector<SkillUri>& skillUris)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    ExecuteVerifyTask(appVerifyBaseInfo, skillUris, TaskType::IMMEDIATE_TASK);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}

void AppDomainVerifyAgentService::ExecuteVerifyTask(
    const AppVerifyBaseInfo& appVerifyBaseInfo, const std::vector<SkillUri>& skillUris, TaskType type)
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
    task->Execute();
}

void AppDomainVerifyAgentService::QueryAndCompleteRefresh(
    const std::vector<InnerVerifyStatus>& statuses, int delaySeconds, TaskType type)
{
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    if (AppDomainVerifyMgrClient::GetInstance()->QueryAllDomainVerifyStatus(bundleVerifyStatusInfo)) {
        CompleteVerifyRefresh(bundleVerifyStatusInfo, statuses, delaySeconds, type);
    }
}

// sa_main进程统一调用
void AppDomainVerifyAgentService::OnStart(const SystemAbilityOnDemandReason& startReason)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnStart reason %{public}s, reasonId_:%{public}d",
        startReason.GetName().c_str(), startReason.GetId());
    if (continuationHandler_ == nullptr) {
        continuationHandler_ = std::make_shared<ffrt::queue>("VerifyServiceContinuationMgr");
    }

    if (startReason.GetName() == BOOT_COMPLETED_EVENT || startReason.GetName() == LOOP_EVENT) {
        TaskType type = startReason.GetName() == BOOT_COMPLETED_EVENT ?
            TaskType::BOOT_REFRESH_TASK :
            TaskType::SCHEDULE_REFRESH_TASK;
        DoSync(type);
    }

    AppDomainVerifyAgentServiceStub::PostDelayUnloadTask();
    bool res = Publish(this);
    if (!res) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "Publish failed");
    } else {
        if (IsInOOBE()) {
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnStart is in OOBE, needDoSync.");
            needCheckOOBE = true;
        }
    }
}
void AppDomainVerifyAgentService::DoSync(const TaskType& type)
{
    auto func = [this, type]() {
        QueryAndCompleteRefresh(
            std::vector<InnerVerifyStatus>{ UNKNOWN, STATE_FAIL, FAILURE_REDIRECT, FAILURE_CLIENT_ERROR,
                FAILURE_REJECTED_BY_SERVER, FAILURE_HTTP_UNKNOWN, FAILURE_TIMEOUT, FAILURE_CONFIG },
            0, type);
    };
    auto updateWhiteListFunc = [this]() { UpdateWhiteList(); };
    continuationHandler_->submit(updateWhiteListFunc);
    continuationHandler_->submit(func);
}

void AppDomainVerifyAgentService::OnStop()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
}
bool AppDomainVerifyAgentService::ShouldRejectUnloadWhenOOBE()
{
    if (needCheckOOBE) {
        if (IsInOOBE()) {
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnIdle is in OOBE, delay unload.");
            needCheckOOBE = true;
            return true;
        }
        DoSync(EnumTaskType::BOOT_REFRESH_TASK);
        needCheckOOBE = false;
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnIdle do sync submit, delay unload once.");
        return true;
    } else {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnIdle no need check oobe status.");
        return false;
    }
}
int32_t AppDomainVerifyAgentService::OnIdle(const SystemAbilityOnDemandReason& idleReason)
{
    APP_DOMAIN_VERIFY_HILOGI(
        APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnIdle reason:%{public}s", idleReason.GetName().c_str());
    if (ShouldRejectUnloadWhenOOBE()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnIdle oobe, delay unload");
        return UNLOAD_DELAY_TIME;
    }
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

void AppDomainVerifyAgentService::UpdateWhiteList()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND != appDomainVerifyExtMgr_->UpdateWhiteList()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
    }
}
void AppDomainVerifyAgentService::OnDump()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnDump");
}

int AppDomainVerifyAgentService::Dump(int fd, const std::vector<std::u16string>& args)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "Dump");
    std::string dumpString{};
    dumpString.append("Running state: ");
    IsIdle() ? dumpString.append("idle.") : dumpString.append("running.");
    dumpString.append("\n");
    (void)write(fd, dumpString.c_str(), dumpString.size());
    return 0;
}
bool AppDomainVerifyAgentService::IsInOOBE()
{
    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    std::string device_provisioned{ "0" };
    OHOS::Uri uri(
        "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=device_provisioned");
    int resp = datashareHelper->Query(uri, "device_provisioned", device_provisioned);
    if (resp == 0 && (device_provisioned == "0" || device_provisioned.empty())) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "IsInOOBE: device_provisioned = 0");
        return true;
    }

    std::string user_setup_complete{ "1" };
    std::vector<int> activedOsAccountIds;
    OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(activedOsAccountIds);
    if (activedOsAccountIds.empty()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "IsInOOBE: activedOsAccountIds is empty");
        return false;
    }
    int userId = activedOsAccountIds[0];
    OHOS::Uri uri_setup("datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_SECURE_" +
        std::to_string(userId) + "?Proxy=true&key=user_setup_complete");
    int resp_userSetup = datashareHelper->Query(uri_setup, "user_setup_complete", user_setup_complete);
    if (resp_userSetup == 0 && (user_setup_complete == "0" || user_setup_complete.empty())) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "IsInOOBE: user_setup_complete = 0");
        return true;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "IsInOOBE: complete");
    return false;
}
}  // namespace AppDomainVerify
}  // namespace OHOS
