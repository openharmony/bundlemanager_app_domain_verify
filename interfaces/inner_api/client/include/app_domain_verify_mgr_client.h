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

#ifndef APP_DOMAIN_VERIFY_MGR_CLIENT_H
#define APP_DOMAIN_VERIFY_MGR_CLIENT_H

#include <memory>
#include "singleton.h"

#include "app_domain_verify_mgr_service_proxy.h"
#include "errors.h"
#include "i_app_domain_verify_mgr_service.h"
#include "skill_uri.h"
#include "domain_verify_status.h"
#include "want.h"

namespace OHOS {
namespace AppDomainVerify {

class AppDomainVerifyMgrClient : public DelayedSingleton<AppDomainVerifyMgrClient> {
    DECLARE_DELAYED_SINGLETON(AppDomainVerifyMgrClient);

public:
    DISALLOW_COPY_AND_MOVE(AppDomainVerifyMgrClient);
    /**
     * VerifyDomain
     * @descrition verify applink in skillUris.
     * @param appIdentifier appIdentifier.
     * @param bundleName bundleName.
     * @param fingerprint fingerprint.
     * @param skillUris skillUris.
     */
    void VerifyDomain(const std::string& appIdentifier, const std::string& bundleName, const std::string& fingerprint,
        const std::vector<SkillUri>& skillUris);

    /**
     * ClearDomainVerifyStatus
     * @descrition delete domain name verification result.
     * @param appIdentifier appIdentifier.
     * @param bundleName bundleName.
     * @return bool clear result.
     */
    bool ClearDomainVerifyStatus(const std::string& appIdentifier, const std::string& bundleName);

    /**
     * FilterAbilities
     * @descrition Perform a further filtering on the ability list based on the domain verification.
     * @param want want to start ability.
     * @param originAbilityInfos AbilityInfo vector input.
     * @param filtedAbilityInfos AbilityInfo vector output.
     * @return bool filterAbilities success or not.
     */
    bool FilterAbilities(const OHOS::AAFwk::Want& want,
        const std::vector<OHOS::AppExecFwk::AbilityInfo>& originAbilityInfos,
        std::vector<OHOS::AppExecFwk::AbilityInfo>& filtedAbilityInfos);

    /**
     * QueryDomainVerifyStatus
     * @descrition query domain verify status.
     * @param bundleName bundleName.
     * @param domainVerificationState domainVerificationState.
     * @return bool query success or not.
     */
    bool QueryDomainVerifyStatus(const std::string& bundleName, DomainVerifyStatus& domainVerificationState);

    /**
     * QueryAllDomainVerifyStatus
     * @descrition QueryAllDomainVerifyStatus.
     * @param bundleVerifyStatusInfo bundleVerifyStatusInfo.
     * @return bool query success or not.
     */
    bool QueryAllDomainVerifyStatus(BundleVerifyStatusInfo& bundleVerifyStatusInfo);

    /**
     * SaveDomainVerifyStatus
     * @descrition SaveDomainVerifyStatus.
     * @param bundleName bundleName.
     * @param verifyResultInfo verifyResultInfo.
     * @return bool query success or not.
     */
    bool SaveDomainVerifyStatus(const std::string& bundleName, const VerifyResultInfo& verifyResultInfo);

    /**
     * IsAtomicServiceUrl
     * @descrition check input url is atomic service or not.
     * @param url input url to check.
     * @return bool is atomic service or not.
     */
    bool IsAtomicServiceUrl(const std::string& url);

    /**
     * UpdateWhiteListUrl
     * @descrition update whitelist urls.
     * @param urls input whitelist urls.
     */
    void UpdateWhiteListUrls(const std::vector<std::string>& urls);
    /**
     * ConvertToExplicitWant
     * @descrition convert implicit want to explicit want.
     * @param implicitWant implicit want to convert.
     * @param callback callback when convert finish.
     */
    void ConvertToExplicitWant(OHOS::AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback);

    /**
     * QueryAssociatedDomains
     * @descrition query domains associated to bundle name.
     * @param bundleName bundleName as key.
     * @param domains domains as result.
     * @return int result of query.
     */
    int QueryAssociatedDomains(const std::string& bundleName, std::vector<std::string>& domains);
    /**
     * QueryAssociatedBundleNames
     * @descrition query bundleNames associated to domain.
     * @param bundleName domain as key.
     * @param bundleNames domains as result.
     * @return int result of query.
     */
    int QueryAssociatedBundleNames(const std::string& domain, std::vector<std::string>& bundleNames);

    /**
    * QueryAppDetailsWant
    * @descrition query app details as want.
    * @param link uri.
    * @param want appdetails want.
    * @return int result of query.
    */
    int QueryAppDetailsWant(const std::string& link, AAFwk::Want& want);

    /**
     * GetDeferredLink
     * @descrition get deferred link for app.
     * @param link link to get.
     * @return result status.
     */
    int GetDeferredLink(std::string& link);

    /**
     * OnRemoteSaDied
     * @descrition
     * @param object systemAbility proxy object
     * @return void.
     */
    void OnRemoteSaDied(const wptr<IRemoteObject>& object);

private:
    bool IsServiceAvailable();
    void ConnectService();
    bool IsValidUrl(OHOS::Uri& uri);
    bool IsValidPath(const std::string& url);

private:
    class StaticDestoryMonitor {
    public:
        StaticDestoryMonitor() : destoryed_(false)
        {
        }
        ~StaticDestoryMonitor()
        {
            destoryed_ = true;
        }

        bool IsDestoryed() const
        {
            return destoryed_;
        }

    private:
        bool destoryed_;
    };

private:
    static std::mutex proxyLock_;
    static sptr<IAppDomainVerifyMgrService> appDomainVerifyMgrServiceProxy_;
    static StaticDestoryMonitor staticDestoryMonitor_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;
};

class AppDomainVerifyMgrSaDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit AppDomainVerifyMgrSaDeathRecipient();
    virtual ~AppDomainVerifyMgrSaDeathRecipient();
    void OnRemoteDied(const wptr<IRemoteObject>& object) override;

private:
    DISALLOW_COPY_AND_MOVE(AppDomainVerifyMgrSaDeathRecipient);
};
}  // namespace AppDomainVerify
}  // namespace OHOS

#endif