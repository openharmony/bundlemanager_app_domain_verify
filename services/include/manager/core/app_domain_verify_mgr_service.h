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

#ifndef APP_DOMAIN_VERIFY_MGR_SERVICE_H
#define APP_DOMAIN_VERIFY_MGR_SERVICE_H

#include "app_domain_verify_mgr_service_stub.h"
#include "inner_verify_status.h"
#include "skill_uri.h"
#include "system_ability.h"
#include "i_app_domain_verify_mgr_service.h"
#include "i_app_domain_verify_agent_service.h"
#include "app_domain_verify_data_mgr.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyMgrService : public SystemAbility, public AppDomainVerifyMgrServiceStub {
    DECLARE_SYSTEM_ABILITY(AppDomainVerifyMgrService);

public:
    API_EXPORT AppDomainVerifyMgrService();
    API_EXPORT virtual ~AppDomainVerifyMgrService();
    API_EXPORT void VerifyDomain(const std::string &appIdentifier, const std::string &bundleName,
        const std::string &fingerprint, const std::vector<SkillUri> &skillUris) override;
    API_EXPORT bool ClearDomainVerifyStatus(const std::string &appIdentifier, const std::string &bundleName) override;
    API_EXPORT bool FilterAbilities(const OHOS::AAFwk::Want &want,
        const std::vector<OHOS::AppExecFwk::AbilityInfo> &originAbilityInfos,
        std::vector<OHOS::AppExecFwk::AbilityInfo> &filtedAbilityInfos) override;
    API_EXPORT bool QueryDomainVerifyStatus(const std::string &bundleName,
        DomainVerifyStatus &domainVerificationState) override;
    API_EXPORT bool QueryAllDomainVerifyStatus(BundleVerifyStatusInfo &bundleVerifyStatusInfo) override;
    API_EXPORT bool SaveDomainVerifyStatus(const std::string &bundleName,
        const VerifyResultInfo &verifyResultInfo) override;

protected:
    void OnDump() override;
    int Dump(int fd, const std::vector<std::u16string>& args) override;
    void OnStart() override;
    void OnStop() override;

private:
    void DumpAllVerifyInfos(std::string& dumpString);
    bool IsWantImplicit(const OHOS::AAFwk::Want &want);

private:
    std::shared_ptr<AppDomainVerifyDataMgr> dataManager_ = nullptr;
};
}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_SERVICE_H