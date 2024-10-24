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
#include "white_list_config_mgr.h"
#include "permission_manager.h"
#include "deferred_link_mgr.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyMgrService : public SystemAbility, public AppDomainVerifyMgrServiceStub {
    DECLARE_SYSTEM_ABILITY(AppDomainVerifyMgrService);

public:
    API_EXPORT AppDomainVerifyMgrService();
    API_EXPORT ~AppDomainVerifyMgrService() override;
    API_EXPORT void VerifyDomain(const std::string& appIdentifier, const std::string& bundleName,
        const std::string& fingerprint, const std::vector<SkillUri>& skillUris) override;
    API_EXPORT bool ClearDomainVerifyStatus(const std::string& appIdentifier, const std::string& bundleName) override;
    API_EXPORT bool FilterAbilities(const OHOS::AAFwk::Want& want,
        const std::vector<OHOS::AppExecFwk::AbilityInfo>& originAbilityInfos,
        std::vector<OHOS::AppExecFwk::AbilityInfo>& filteredAbilityInfos) override;
    API_EXPORT bool QueryDomainVerifyStatus(
        const std::string& bundleName, DomainVerifyStatus& domainVerificationState) override;
    API_EXPORT bool QueryAllDomainVerifyStatus(BundleVerifyStatusInfo& bundleVerifyStatusInfo) override;
    API_EXPORT bool SaveDomainVerifyStatus(
        const std::string& bundleName, const VerifyResultInfo& verifyResultInfo) override;
    API_EXPORT bool IsAtomicServiceUrl(const std::string& url) override;
    API_EXPORT void ConvertToExplicitWant(OHOS::AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback) override;
    API_EXPORT void UpdateWhiteListUrls(const std::vector<std::string>& urls) override;
    API_EXPORT int QueryAssociatedDomains(const std::string& bundleName, std::vector<std::string>& domains) override;
    API_EXPORT int QueryAssociatedBundleNames(
        const std::string& domain, std::vector<std::string>& bundleNames) override;
    API_EXPORT int GetDeferredLink(std::string& link) override;

protected:
    void OnDump() override;
    int Dump(int fd, const std::vector<std::u16string>& args) override;
    void OnStart() override;
    void OnStop() override;

private:
    void DumpAllVerifyInfos(std::string& dumpString);
    bool IsWantImplicit(const OHOS::AAFwk::Want& want);
    static int CheckPermission();
    static void CollectDomains(const std::vector<SkillUri>& skillUris, VerifyResultInfo& verifyResultInfo);

private:
    std::shared_ptr<AppDomainVerifyDataMgr> dataManager_ = nullptr;
    bool InitConfigMgr();
    std::shared_ptr<WhiteListConfigMgr> whiteListConfigMgr_;
    std::mutex initConfigMutex_;
    std::shared_ptr<DeferredLinkMgr> deferredLinkMgr_ = std::make_shared<DeferredLinkMgr>();
};
}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_MGR_SERVICE_H