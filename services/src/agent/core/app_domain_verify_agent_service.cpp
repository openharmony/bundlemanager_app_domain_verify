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
#include "net_conn_client.h"
#include "iservice_registry.h"

namespace OHOS {
namespace AppDomainVerify {
namespace {
constexpr int32_t DELAY_TIME = 300000;  // 5min = 5*60*1000
std::atomic<int> retryCnt = 0;
std::atomic<bool> isDoSyncDone = false;
constexpr int MAX_NET_RETRY_CNT = 3;
}
static const std::string TASK_ID = "unload";
static const std::string BOOT_COMPLETED_EVENT = "usual.event.BOOT_COMPLETED";
static const std::string LOOP_EVENT = "loopevent";

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(new AppDomainVerifyAgentService());

AppDomainVerifyAgentService::AppDomainVerifyAgentService() : SystemAbility(APP_DOMAIN_VERIFY_AGENT_SA_ID, true)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "new instance create.");
    appDomainVerifyExtMgr_ = std::make_shared<AppDomainVerifyExtensionMgr>();
    appDomainVerifyTaskMgr_ = AppDomainVerifyTaskMgr::GetInstance();
    runner_ = AppExecFwk::EventRunner::Create("unload", AppExecFwk::ThreadMode::FFRT);
    if (runner_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "create runner failed.");
        return;
    }
    unloadHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
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
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
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
        AppVerifyBaseInfo appVerifyBaseInfo;
        appVerifyBaseInfo.bundleName = it->first;
        if (appVerifyBaseInfo.appIdentifier.empty() &&
            !BundleInfoQuery::GetBundleInfo(
                appVerifyBaseInfo.bundleName, appVerifyBaseInfo.appIdentifier, appVerifyBaseInfo.fingerprint)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "GetBundleInfo failed.");
            // todo delete this bundleName or not
            continue;
        }
        std::vector<SkillUri> skillUris;
        for (auto itr = it->second.hostVerifyStatusMap.begin(); itr != it->second.hostVerifyStatusMap.end(); itr++) {
            if (std::find(statuses.begin(), statuses.end(), itr->second) != statuses.end()) {
                SkillUri skillUri;
                skillUri.host = UrlUtil::GetHost(itr->first);
                skillUri.scheme = UrlUtil::GetScheme(itr->first);
                skillUris.emplace_back(skillUri);
            }
        }
        ExecuteVerifyTask(appVerifyBaseInfo, skillUris, type);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "call end");
}
void AppDomainVerifyAgentService::ConvertToExplicitWant(
    OHOS::AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND !=
        appDomainVerifyExtMgr_->ConvertToExplicitWant(implicitWant, callback)) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
    }

    if (callback) {
        callback->OnConvert(0, implicitWant);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
}
void AppDomainVerifyAgentService::SingleVerify(
    const AppVerifyBaseInfo& appVerifyBaseInfo, const std::vector<SkillUri>& skillUris)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    ExecuteVerifyTask(appVerifyBaseInfo, skillUris, TaskType::IMMEDIATE_TASK);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "call end");
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
void AppDomainVerifyAgentService::UpdateWhiteList()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND != appDomainVerifyExtMgr_->UpdateWhiteList()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
    }
}
// sa_main进程统一调用
void AppDomainVerifyAgentService::OnStart(const SystemAbilityOnDemandReason& startReason)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnStart reason %{public}s, reasonId_:%{public}d",
        startReason.GetName().c_str(), startReason.GetId());
    PostDelayUnloadTask();
    bool res = Publish(this);
    if (!res) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "Publish failed");
    }
}

void AppDomainVerifyAgentService::OnStop()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
}

void AppDomainVerifyAgentService::DoSync(const TaskType& type)
{
    QueryAndCompleteRefresh(std::vector<InnerVerifyStatus>{ UNKNOWN, STATE_FAIL, FAILURE_REDIRECT, FAILURE_CLIENT_ERROR,
                                FAILURE_REJECTED_BY_SERVER, FAILURE_HTTP_UNKNOWN, FAILURE_TIMEOUT, FAILURE_CONFIG },
        0, type);
    UpdateWhiteList();
}
bool AppDomainVerifyAgentService::IsIdle()
{
    if (appDomainVerifyTaskMgr_ == nullptr) {
        return true;
    } else {
        return appDomainVerifyTaskMgr_->IsIdle();
    }
}

bool AppDomainVerifyAgentService::IsNetAvaliable()
{
    bool IsNetAvailable = false;
    NetManagerStandard::NetConnClient::GetInstance().HasDefaultNet(IsNetAvailable);
    return IsNetAvailable;
}

bool AppDomainVerifyAgentService::CanUnloadSa()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    // 尝试进行最大次数的有网络同步操作
    if (!isDoSyncDone && retryCnt++ < MAX_NET_RETRY_CNT) {
        if (IsNetAvaliable()) {
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "net avaliable, do sync");
            DoSync(BOOT_REFRESH_TASK);
            isDoSyncDone = true;
        } else {
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "net un avaliable, retry and delay");
            return false;
        }
    }
    // 还需要判断是否有网络任务
    if (IsIdle()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "task idle");
        return true;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "task not idle");
    return false;
}
void AppDomainVerifyAgentService::UnloadSa()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "do unload sa");
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "get samgr failed");
        return;
    }
    int32_t ret = samgrProxy->UnloadSystemAbility(APP_DOMAIN_VERIFY_AGENT_SA_ID);
    if (ret != 0) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "remove system ability failed");
        return;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "do unload sa done");
}
void AppDomainVerifyAgentService::OnDelayUnloadSA()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "on unload task");
    if (!CanUnloadSa()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "can not unload sa, delay unload");
        unloadHandler_->RemoveTask(TASK_ID);
        unloadHandler_->PostTask([this] { PostDelayUnloadTask(); }, TASK_ID, DELAY_TIME);
        return;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "on unload task, do unload");
    UnloadSa();
}
void AppDomainVerifyAgentService::PostDelayUnloadTask()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    unloadHandler_->RemoveTask(TASK_ID);
    unloadHandler_->PostTask([this] { OnDelayUnloadSA(); }, TASK_ID, DELAY_TIME);
}

void AppDomainVerifyAgentService::ExitIdleState()
{
    CancelIdle();
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

}  // namespace AppDomainVerify
}  // namespace OHOS
