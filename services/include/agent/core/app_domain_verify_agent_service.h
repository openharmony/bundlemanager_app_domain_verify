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

#ifndef APP_DOMAIN_VERIFY_AGENT_SERVICE_H
#define APP_DOMAIN_VERIFY_AGENT_SERVICE_H

#include "app_domain_verify_agent_service_stub.h"
#include <chrono>
#include "inner_verify_status.h"
#include "skill_uri.h"
#include "system_ability.h"
#include "ffrt.h"
#include "app_domain_verify_extension_mgr.h"
#include "i_app_domain_verify_mgr_service.h"
#include "app_domain_verify_task_mgr.h"
#include "dfx/app_domain_verify_hisysevent.h"
#include "event_handler.h"
#include "event_runner.h"
#include "net_conn_client.h"

namespace OHOS {
namespace AppDomainVerify {
using namespace NetManagerStandard;
class AppDomainVerifyAgentService : public SystemAbility, public AppDomainVerifyAgentServiceStub {
    DECLARE_SYSTEM_ABILITY(AppDomainVerifyAgentService);

public:
    API_EXPORT AppDomainVerifyAgentService();
    API_EXPORT ~AppDomainVerifyAgentService() override;
    API_EXPORT void SingleVerify(
        const AppVerifyBaseInfo& appVerifyBaseInfo, const VerifyResultInfo &verifyResultInfo) override;
    API_EXPORT void ConvertToExplicitWant(OHOS::AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback) override;
protected:
    void OnStart(const SystemAbilityOnDemandReason& startReason) override;
    void OnStop() override;
    void OnDump() override;
    int Dump(int fd, const std::vector<std::u16string>& args) override;

private:
    void QueryAndCompleteRefresh(TaskType type);
    void CompleteVerifyRefresh(const BundleVerifyStatusInfo& bundleVerifyStatusInfo, TaskType type);
    void ExecuteVerifyTask(
        const AppVerifyBaseInfo& appVerifyBaseInfo, const VerifyResultInfo &verifyResultInfo, TaskType type);
    void UpdateWhiteList();
    void UpdateAppDetails();
    void OnDelayUnloadSA();
    void PostDelayUnloadTask();
    void PostUpdateAppDetailsTask();
    void DoSync(const TaskType& type);
    void DoSync();
    bool IsIdle();
    bool IsNetAvailable();
    bool CanUnloadSa();
    void UnloadSa();
    std::string GetStatTime();

private:
    std::shared_ptr<AppDomainVerifyExtensionMgr> appDomainVerifyExtMgr_;
    std::shared_ptr<AppDomainVerifyTaskMgr> appDomainVerifyTaskMgr_;
    std::shared_ptr<AppExecFwk::EventHandler> unloadHandler_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::chrono::system_clock::time_point now;
    std::atomic<bool> isUpdateRunning_ = false;
};

}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_SERVICE_H