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
#include "iservice_registry.h"
#include "app_domain_verify_error.h"
#include "permission_manager.h"

namespace OHOS {
namespace AppDomainVerify {
namespace {
#ifndef _TEST
constexpr int32_t DELAY_TIME = 180000;  // 3min = 3*60*1000
constexpr int MAX_DELAY_RETRY_CNT = 10;
#elif
constexpr int32_t DELAY_TIME = 60000;   // 1min = 60*1000 for test
constexpr int MAX_DELAY_RETRY_CNT = 3;  // 3 for test
#endif
std::atomic<int> retryCnt = 0;
std::atomic<bool> isDoSyncDone = false;
constexpr int32_t DUMP_SYSTEM_START_YEAR = 1900;
constexpr int32_t FORMAT_BLANK_SIZE = 32;
}
static const std::string TASK_ID = "unload";
static const std::string UPDATE_DETAILS_TASK_ID = "udpateDetails";
using namespace NetManagerStandard;
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(new AppDomainVerifyAgentService());

AppDomainVerifyAgentService::AppDomainVerifyAgentService() : SystemAbility(APP_DOMAIN_VERIFY_AGENT_SA_ID, true)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "new instance create.");
    now = std::chrono::system_clock::now();
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
void AppDomainVerifyAgentService::CompleteVerifyRefresh(
    const BundleVerifyStatusInfo& bundleVerifyStatusInfo, TaskType type)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND !=
        appDomainVerifyExtMgr_->CompleteVerifyRefresh(bundleVerifyStatusInfo, type)) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
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
        ExecuteVerifyTask(appVerifyBaseInfo, it->second, type);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "call end");
}
void AppDomainVerifyAgentService::ConvertToExplicitWant(
    OHOS::AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if(PermissionManager::IsSACall()){
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "no sa call");
        return;
    }
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
    const AppVerifyBaseInfo& appVerifyBaseInfo, const VerifyResultInfo& verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    if(PermissionManager::IsSACall()){
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "no sa call");
        return;
    }
    ExecuteVerifyTask(appVerifyBaseInfo, verifyResultInfo, TaskType::IMMEDIATE_TASK);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "call end");
}

void AppDomainVerifyAgentService::ExecuteVerifyTask(
    const AppVerifyBaseInfo& appVerifyBaseInfo, const VerifyResultInfo& verifyResultInfo, TaskType type)
{
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND !=
        appDomainVerifyExtMgr_->SingleVerify(appVerifyBaseInfo, verifyResultInfo)) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
    }

    auto task = std::make_shared<VerifyTask>(type, appVerifyBaseInfo, verifyResultInfo);
    if (task->GetUriVerifyMap().empty()) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "no valid skillUris");
        return;
    }
    task->Execute();
}

void AppDomainVerifyAgentService::QueryAndCompleteRefresh(TaskType type)
{
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    if (AppDomainVerifyMgrClient::GetInstance()->QueryAllDomainVerifyStatus(bundleVerifyStatusInfo)) {
        CompleteVerifyRefresh(bundleVerifyStatusInfo, type);
        return;
    }
    UNIVERSAL_ERROR_EVENT(GET_DATE_IN_BOOT_FAULT);
}

void AppDomainVerifyAgentService::UpdateWhiteList()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND != appDomainVerifyExtMgr_->UpdateWhiteList()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
    }
}

void AppDomainVerifyAgentService::UpdateAppDetails()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND != appDomainVerifyExtMgr_->UpdateAppDetails()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return;
    }
}
int AppDomainVerifyAgentService::CommonTransact(uint32_t opcode, const std::string& request, std::string& response)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    if (ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND != appDomainVerifyExtMgr_->CommonTransact(opcode, request, response)) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "extension call end");
        return ErrorCode::E_INTERNAL_ERR;
    }
    return ErrorCode::E_OK;
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

bool AppDomainVerifyAgentService::IsIdle()
{
    if (appDomainVerifyTaskMgr_ == nullptr) {
        return true;
    } else {
        return appDomainVerifyTaskMgr_->IsIdle();
    }
}

void AppDomainVerifyAgentService::DoSync(const TaskType& type)
{
    QueryAndCompleteRefresh(type);
    UpdateWhiteList();
    UpdateAppDetails();
}

bool AppDomainVerifyAgentService::IsNetAvailable()
{
    bool isNetAvailable = false;
    NetManagerStandard::NetConnClient::GetInstance().HasDefaultNet(isNetAvailable);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "IsNetAvailable:%{public}d", isNetAvailable);
    return isNetAvailable;
}

void AppDomainVerifyAgentService::DoSync()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "do sync");
    if (!isDoSyncDone && IsNetAvailable()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "net connected, do sync once");
        DoSync(BOOT_REFRESH_TASK);
        isDoSyncDone = true;
    }
}

bool AppDomainVerifyAgentService::CanUnloadSa()
{
    auto reachedMaxCnt = (retryCnt >= MAX_DELAY_RETRY_CNT - 1);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE,
        "can unload? isDoSyncDone:%{public}d, retryCnt:%{public}d, IsIdle:%{public}d, reachedMaxCnt:%{public}d, "
        "maxCnt:%{public}d",
        isDoSyncDone.load(), retryCnt.load(), IsIdle(), reachedMaxCnt, MAX_DELAY_RETRY_CNT);
    return (isDoSyncDone || reachedMaxCnt) && IsIdle();
}

void AppDomainVerifyAgentService::OnDelayUnloadSA()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "on unload task");
    if (CanUnloadSa()) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "can unload sa");
        UnloadSa();
        return;
    }

    DoSync();
    PostDelayUnloadTask();
    retryCnt++;
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "on unload task, delay unload");
}
void AppDomainVerifyAgentService::PostDelayUnloadTask()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    unloadHandler_->RemoveTask(TASK_ID);
    unloadHandler_->PostTask([this] { OnDelayUnloadSA(); }, TASK_ID, DELAY_TIME);
}

void AppDomainVerifyAgentService::OnDump()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnDump");
}
std::string AppDomainVerifyAgentService::GetStatTime()
{
    std::string startTime;
    time_t tt = std::chrono::system_clock::to_time_t(now);
    auto ptm = localtime(&tt);
    if (ptm != nullptr) {
        char date[FORMAT_BLANK_SIZE] = { 0 };
        auto flag = sprintf_s(date, sizeof(date), "%04d-%02d-%02d  %02d:%02d:%02d",
            (int)ptm->tm_year + DUMP_SYSTEM_START_YEAR, (int)ptm->tm_mon + 1, (int)ptm->tm_mday, (int)ptm->tm_hour,
            (int)ptm->tm_min, (int)ptm->tm_sec);
        if (flag < 0) {
            return startTime;
        }
        startTime = date;
    }
    return startTime;
}
int AppDomainVerifyAgentService::Dump(int fd, const std::vector<std::u16string>& args)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "Dump");
    std::string dumpString{};
    dumpString.append("Agent start time: ");
    dumpString.append(GetStatTime());
    dumpString.append("\n");
    dumpString.append("TaskMgr state: ");
    IsIdle() ? dumpString.append("idle.") : dumpString.append("running.");
    dumpString.append("\n");
    dumpString.append("isDoSyncDone:");
    isDoSyncDone ? dumpString.append("true.") : dumpString.append("false.");
    dumpString.append("\n");
    dumpString.append("retryCnt:");
    dumpString.append(std::to_string(retryCnt));
    dumpString.append("\n");
    dumpString.append("maxCnt:");
    dumpString.append(std::to_string(MAX_DELAY_RETRY_CNT));
    dumpString.append("\n");
    (void)write(fd, dumpString.c_str(), dumpString.size());
    return 0;
}

}  // namespace AppDomainVerify
}  // namespace OHOS
