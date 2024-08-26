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
#include "inner_verify_status.h"
#include "skill_uri.h"
#include "system_ability.h"
#include "ffrt.h"
#include "app_domain_verify_extension_mgr.h"
#include "i_app_domain_verify_mgr_service.h"
#include "app_domain_verify_task_mgr.h"
#include "dfx/app_domain_verify_hisysevent.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyAgentService : public SystemAbility, public AppDomainVerifyAgentServiceStub {
    DECLARE_SYSTEM_ABILITY(AppDomainVerifyAgentService);

public:
    API_EXPORT AppDomainVerifyAgentService();
    API_EXPORT virtual ~AppDomainVerifyAgentService();
    API_EXPORT void SingleVerify(
        const AppVerifyBaseInfo& appVerifyBaseInfo, const std::vector<SkillUri>& skillUris) override;
    API_EXPORT void ConvertToExplicitWant(OHOS::AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback) override;

protected:
    void OnStart(const SystemAbilityOnDemandReason& startReason) override;
    void OnStop() override;
    int32_t OnIdle(const SystemAbilityOnDemandReason& idleReason) override;
    void ExitIdleState() override;
    void OnDump() override;
    int Dump(int fd, const std::vector<std::u16string>& args) override;

private:
    void QueryAndCompleteRefresh(const std::vector<InnerVerifyStatus>& statuses, int delaySeconds, TaskType type);
    void CompleteVerifyRefresh(const BundleVerifyStatusInfo& bundleVerifyStatusInfo,
        const std::vector<InnerVerifyStatus>& statuses, int delaySeconds, TaskType type);
    void ExecuteVerifyTask(
        const AppVerifyBaseInfo& appVerifyBaseInfo, const std::vector<SkillUri>& skillUris, TaskType type);
    bool IsIdle();
    void UpdateWhiteList();

private:
    std::shared_ptr<ffrt::queue> continuationHandler_;
    std::shared_ptr<AppDomainVerifyExtensionMgr> appDomainVerifyExtMgr_;
    std::shared_ptr<AppDomainVerifyTaskMgr> appDomainVerifyTaskMgr_;
    bool IsInOOBE();
    void DoSync(const TaskType& type);
    bool ShouldRejectUnloadWhenOOBE();
};

}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_SERVICE_H