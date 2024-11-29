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

#ifndef APP_DOMAIN_VERIFY_MGR_SERVICE_PROXY_H
#define APP_DOMAIN_VERIFY_MGR_SERVICE_PROXY_H

#include "i_app_domain_verify_mgr_service.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyMgrServiceProxy : public IRemoteProxy<IAppDomainVerifyMgrService> {
public:
    explicit AppDomainVerifyMgrServiceProxy(const sptr<IRemoteObject>& object);
    ~AppDomainVerifyMgrServiceProxy() override;
    void VerifyDomain(const std::string& appIdentifier, const std::string& bundleName,
        const std::string& fingerprint, const std::vector<SkillUri>& skillUris) override;
    bool ClearDomainVerifyStatus(const std::string& appIdentifier, const std::string& bundleName) override;
    bool FilterAbilities(const OHOS::AAFwk::Want& want,
        const std::vector<OHOS::AppExecFwk::AbilityInfo>& originAbilityInfos,
        std::vector<OHOS::AppExecFwk::AbilityInfo>& filteredAbilityInfos) override;
    bool QueryDomainVerifyStatus(
        const std::string& bundleName, DomainVerifyStatus& domainVerificationState) override;
    bool QueryAllDomainVerifyStatus(BundleVerifyStatusInfo& bundleVerifyStatusInfo) override;
    bool SaveDomainVerifyStatus(
        const std::string& bundleName, const VerifyResultInfo& verifyResultInfo) override;
    bool IsAtomicServiceUrl(const std::string& url) override;
    void ConvertToExplicitWant(OHOS::AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback) override;
    void UpdateWhiteListUrls(const std::vector<std::string>& urls) override;
    int QueryAssociatedDomains(const std::string& bundleName, std::vector<std::string>& domains) override;
    int QueryAssociatedBundleNames(const std::string& domain, std::vector<std::string>& bundleNames) override;
    int GetDeferredLink(std::string& link) override;
private:
    static inline BrokerDelegator<AppDomainVerifyMgrServiceProxy> delegator_;
};
}  // namespace AppDomainVerify
}  // namespace OHOS

#endif  // APP_DOMAIN_VERIFY_SERVICE_PROXY_H