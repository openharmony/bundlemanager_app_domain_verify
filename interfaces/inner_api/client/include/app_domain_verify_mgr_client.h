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
    void VerifyDomain(const std::string &appIdentifier, const std::string &bundleName, const std::string &fingerprint,
        const std::vector<SkillUri> &skillUris);

    /**
     * ClearDomainVerifyStatus
     * @descrition delete domain name verification result.
     * @param appIdentifier appIdentifier.
     * @param bundleName bundleName.
     * @return bool clear result.
     */
    bool ClearDomainVerifyStatus(const std::string &appIdentifier, const std::string &bundleName);

    /**
     * FilterAbilities
     * @descrition Perform a further filtering on the ability list based on the domain verification.
     * @param want want to start ability.
     * @param originAbilityInfos AbilityInfo vector input.
     * @param filtedAbilityInfos AbilityInfo vector output.
     * @return bool filterAbilities success or not.
     */
    bool FilterAbilities(const OHOS::AAFwk::Want &want,
        const std::vector<OHOS::AppExecFwk::AbilityInfo> &originAbilityInfos,
        std::vector<OHOS::AppExecFwk::AbilityInfo> &filtedAbilityInfos);

    /**
     * QueryDomainVerifyStatus
     * @descrition query domain verify status.
     * @param bundleName bundleName.
     * @param domainVerificationState domainVerificationState.
     * @return bool query success or not.
     */
    bool QueryDomainVerifyStatus(const std::string &bundleName, DomainVerifyStatus &domainVerificationState);

    /**
     * QueryAllDomainVerifyStatus
     * @descrition QueryAllDomainVerifyStatus.
     * @param bundleVerifyStatusInfo bundleVerifyStatusInfo.
     * @return bool query success or not.
     */
    bool QueryAllDomainVerifyStatus(BundleVerifyStatusInfo &bundleVerifyStatusInfo);

    /**
     * SaveDomainVerifyStatus
     * @descrition SaveDomainVerifyStatus.
     * @param bundleName bundleName.
     * @param verifyResultInfo verifyResultInfo.
     * @return bool query success or not.
     */
    bool SaveDomainVerifyStatus(const std::string &bundleName, const VerifyResultInfo &verifyResultInfo);

    /**
     * OnRemoteSaDied
     * @descrition
     * @param object systemAbility proxy object
     * @return void.
     */
    void OnRemoteSaDied(const wptr<IRemoteObject> &object);

private:
    bool IsServiceAvailable();
    void ConnectService();

private:
    class StaticDestoryMonitor {
    public:
        StaticDestoryMonitor()
            : destoryed_(false)
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
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;

private:
    DISALLOW_COPY_AND_MOVE(AppDomainVerifyMgrSaDeathRecipient);
};
}  // namespace AppDomainVerify
}  // namespace OHOS

#endif