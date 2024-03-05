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
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "app_domain_verify_client.h"
#include "refbase.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppDomainVerify {
std::mutex AppDomainVerifyClient::proxyLock_;
sptr<IAppDomainVerifyService> AppDomainVerifyClient::appDomainVerifyServiceProxy_;
AppDomainVerifyClient::StaticDestoryMonitor AppDomainVerifyClient::staticDestoryMonitor_;

AppDomainVerifyClient::AppDomainVerifyClient()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "new instance created.");
}
AppDomainVerifyClient::~AppDomainVerifyClient()
{
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (appDomainVerifyServiceProxy_ != nullptr && !staticDestoryMonitor_.IsDestoryed()) {
        auto remoteObject = appDomainVerifyServiceProxy_->AsObject();
        if (remoteObject != nullptr) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
        }
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "instance dead.");
}

void AppDomainVerifyClient::VerifyDomain(const std::string &appIdentifier, const std::string &bundleName,
    const std::string &fingerprint, const std::vector<SkillUri> &skillUris)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s called", __func__);
    if (IsServiceAvailable()) {
        appDomainVerifyServiceProxy_->VerifyDomain(appIdentifier, bundleName, fingerprint, skillUris);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s call end", __func__);
}

bool AppDomainVerifyClient::ClearDomainVerifyStatus(const std::string &appIdentifier, const std::string &bundleName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s called", __func__);
    bool clearResult = false;
    if (IsServiceAvailable()) {
        clearResult = appDomainVerifyServiceProxy_->ClearDomainVerifyStatus(appIdentifier, bundleName);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s call end", __func__);
    return clearResult;
}

bool AppDomainVerifyClient::FilterAbilities(const OHOS::AAFwk::Want &want,
    const std::vector<OHOS::AppExecFwk::AbilityInfo> &originAbilityInfos,
    std::vector<OHOS::AppExecFwk::AbilityInfo> &filtedAbilityInfos)
{  
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s called", __func__);
    bool filterSuccess = false;
    if (IsServiceAvailable()) {
        filterSuccess = appDomainVerifyServiceProxy_->FilterAbilities(want, originAbilityInfos, filtedAbilityInfos);
    }

    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s call end", __func__);
    return filterSuccess;
}

bool AppDomainVerifyClient::QueryDomainVerifyStatus(const std::string &bundleName,
    DomainVerifyStatus &domainVerificationState)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s called", __func__);
    bool querySuccess = false;
    if (IsServiceAvailable()) {
        querySuccess = appDomainVerifyServiceProxy_->QueryDomainVerifyStatus(bundleName, domainVerificationState);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s call end", __func__);
    return querySuccess;
}

bool AppDomainVerifyClient::QueryAllDomainVerifyStatus(BundleVerifyStatusInfo &bundleVerifyStatusInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s called", __func__);
    bool querySuccess = false;
    if (IsServiceAvailable()) {
        querySuccess = appDomainVerifyServiceProxy_->QueryAllDomainVerifyStatus(bundleVerifyStatusInfo);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s call end", __func__);
    return querySuccess;
}

bool AppDomainVerifyClient::SaveDomainVerifyStatus(const std::string &bundleName,
    const VerifyResultInfo &verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s called", __func__);
    bool querySuccess = false;
    if (IsServiceAvailable()) {
        querySuccess = appDomainVerifyServiceProxy_->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "%s call end", __func__);
    return querySuccess;
}

bool AppDomainVerifyClient::IsServiceAvailable()
{
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (appDomainVerifyServiceProxy_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MODULE_CLIENT, "Redo ConnectService");
        ConnectService();
    }

    if (appDomainVerifyServiceProxy_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_CLIENT, "Service proxy null.");
        return false;
    }
    return true;
}
void AppDomainVerifyClient::ConnectService()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "ConnectService start.");
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_CLIENT, "Get SystemAbilityManager failed.");
        appDomainVerifyServiceProxy_ = nullptr;
        return;
    }
    sptr<IRemoteObject> remoteObject = samgrProxy->CheckSystemAbility(APP_DOMAIN_VERIFY_SERVICE_ID);
    if (remoteObject != nullptr) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_CLIENT, "Get AppDomainVerifyServiceProxy succeed.");
        if (deathRecipient_ == nullptr) {
            deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new AppDomainVerifySaDeathRecipient());
        }
        remoteObject->AddDeathRecipient(deathRecipient_);
        appDomainVerifyServiceProxy_ = iface_cast<IAppDomainVerifyService>(remoteObject);
        return;
    }
    APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MODULE_CLIENT, "Getting AppDomainVerifyServiceProxy failed.");
}

void AppDomainVerifyClient::OnRemoteSaDied(const wptr<IRemoteObject> &remote)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_CLIENT, "OnRemoteSaDied.");
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (appDomainVerifyServiceProxy_ != nullptr) {
        auto remoteObj = appDomainVerifyServiceProxy_->AsObject();
        if (remoteObj != nullptr) {
            remoteObj->RemoveDeathRecipient(deathRecipient_);
        }
    }
    appDomainVerifyServiceProxy_ = nullptr;
    ConnectService();
}

AppDomainVerifySaDeathRecipient::AppDomainVerifySaDeathRecipient()
{
}

AppDomainVerifySaDeathRecipient::~AppDomainVerifySaDeathRecipient()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT, "deathRecipient dead.");
}

void AppDomainVerifySaDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_CLIENT,
        "AppDomainVerifySaDeathRecipient on remote systemAbility died.");
    AppDomainVerifyClient::GetInstance()->OnRemoteSaDied(object);
}
}  // namespace AppDomainVerify
}  // namespace OHOS