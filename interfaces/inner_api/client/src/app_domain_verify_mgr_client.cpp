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

#include "app_domain_verify_hilog.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "app_domain_verify_mgr_client.h"
#include "system_ability_definition.h"
#include "dfx/app_domain_verify_hisysevent.h"
#include "zidl/convert_callback_stub.h"
#include "regex.h"
#include "app_domain_verify_error.h"
#include "ipc_skeleton.h"
#include "common_utils.h"
#include "bundle_info_query.h"

namespace OHOS {
namespace AppDomainVerify {
std::mutex AppDomainVerifyMgrClient::proxyLock_;
sptr<IAppDomainVerifyMgrService> AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_;
AppDomainVerifyMgrClient::StaticDestoryMonitor AppDomainVerifyMgrClient::staticDestoryMonitor_;
static const std::string SCHEME_HTTPS("https");
constexpr int MAX_SHORT_URL_LEN = 2048;
AppDomainVerifyMgrClient::AppDomainVerifyMgrClient()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "new instance created.");
}
AppDomainVerifyMgrClient::~AppDomainVerifyMgrClient()
{
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (appDomainVerifyMgrServiceProxy_ != nullptr && !staticDestoryMonitor_.IsDestoryed()) {
        auto remoteObject = appDomainVerifyMgrServiceProxy_->AsObject();
        if (remoteObject != nullptr) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
        }
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "instance dead.");
}

void AppDomainVerifyMgrClient::VerifyDomain(const std::string& appIdentifier, const std::string& bundleName,
    const std::string& fingerprint, const std::vector<SkillUri>& skillUris)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    INSTALL_EVENT(appIdentifier, bundleName);
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        appDomainVerifyMgrServiceProxy_->VerifyDomain(appIdentifier, bundleName, fingerprint, skillUris);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
}

bool AppDomainVerifyMgrClient::ClearDomainVerifyStatus(const std::string& appIdentifier, const std::string& bundleName)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    UNINSTALL_EVENT(appIdentifier, bundleName);
    bool clearResult = false;
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        clearResult = appDomainVerifyMgrServiceProxy_->ClearDomainVerifyStatus(appIdentifier, bundleName);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return clearResult;
}

bool AppDomainVerifyMgrClient::FilterAbilities(const OHOS::AAFwk::Want& want,
    const std::vector<OHOS::AppExecFwk::AbilityInfo>& originAbilityInfos,
    std::vector<OHOS::AppExecFwk::AbilityInfo>& filtedAbilityInfos)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    bool filterSuccess = false;
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        filterSuccess = appDomainVerifyMgrServiceProxy_->FilterAbilities(want, originAbilityInfos, filtedAbilityInfos);
    }

    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT,
        "call end, filted ability size:%{public}zu, origin ability size:%{public}zu", filtedAbilityInfos.size(),
        originAbilityInfos.size());
    return filterSuccess;
}

bool AppDomainVerifyMgrClient::QueryDomainVerifyStatus(
    const std::string& bundleName, DomainVerifyStatus& domainVerificationState)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    bool querySuccess = false;
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        querySuccess = appDomainVerifyMgrServiceProxy_->QueryDomainVerifyStatus(bundleName, domainVerificationState);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return querySuccess;
}

bool AppDomainVerifyMgrClient::QueryAllDomainVerifyStatus(BundleVerifyStatusInfo& bundleVerifyStatusInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    bool querySuccess = false;
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        querySuccess = appDomainVerifyMgrServiceProxy_->QueryAllDomainVerifyStatus(bundleVerifyStatusInfo);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return querySuccess;
}

bool AppDomainVerifyMgrClient::SaveDomainVerifyStatus(
    const std::string& bundleName, const VerifyResultInfo& verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    bool saveSuccess = false;
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        saveSuccess = appDomainVerifyMgrServiceProxy_->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return saveSuccess;
}

bool AppDomainVerifyMgrClient::IsServiceAvailable()
{
    if (appDomainVerifyMgrServiceProxy_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Redo ConnectService");
        ConnectService();
    }

    if (appDomainVerifyMgrServiceProxy_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Service proxy null.");
        UNIVERSAL_ERROR_EVENT(CONNECT_MGR_FAULT);
        return false;
    }
    return true;
}
void AppDomainVerifyMgrClient::ConnectService()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "ConnectService start.");
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "ConnectService SystemAbilityManager failed.");
        appDomainVerifyMgrServiceProxy_ = nullptr;
        return;
    }
    sptr<IRemoteObject> remoteObject = samgrProxy->CheckSystemAbility(APP_DOMAIN_VERIFY_MANAGER_SA_ID);
    if (remoteObject != nullptr) {
        APP_DOMAIN_VERIFY_HILOGI(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "ConnectService AppDomainVerifyMgrServiceProxy succeed.");
        if (deathRecipient_ == nullptr) {
            deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new AppDomainVerifyMgrSaDeathRecipient());
        }
        remoteObject->AddDeathRecipient(deathRecipient_);
        appDomainVerifyMgrServiceProxy_ = iface_cast<IAppDomainVerifyMgrService>(remoteObject);
        return;
    }

    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Getting AppDomainVerifyMgrServiceProxy failed.");
}

void AppDomainVerifyMgrClient::OnRemoteSaDied(const wptr<IRemoteObject>& remote)
{
    APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "OnRemoteSaDied.");
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (appDomainVerifyMgrServiceProxy_ != nullptr) {
        auto remoteObj = appDomainVerifyMgrServiceProxy_->AsObject();
        if (remoteObj != nullptr) {
            remoteObj->RemoveDeathRecipient(deathRecipient_);
        }
    }
    appDomainVerifyMgrServiceProxy_ = nullptr;
}
void AppDomainVerifyMgrClient::ConvertToExplicitWant(AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback)
{
#ifdef _CUT_LINK_CONVERT_
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "not support, will return false!");
    if (callback) {
        callback->OnConvert(-1, implicitWant);
    }
#else
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        appDomainVerifyMgrServiceProxy_->ConvertToExplicitWant(implicitWant, callback);
        IPCSkeleton::SetCallingIdentity(identity);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
#endif
}
#ifndef _CUT_LINK_CONVERT_

bool AppDomainVerifyMgrClient::IsValidUrl(const std::string& url)
{
    if (url.length() > MAX_SHORT_URL_LEN) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "url too long");
        return false;
    }
    Uri uri(url);
    if (uri.GetScheme() != SCHEME_HTTPS) {
        APP_DOMAIN_VERIFY_HILOGW(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "scheme:%{public}s is not https", uri.GetScheme().c_str());
        return false;
    }
    if (uri.GetHost().empty()) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "host is empty");
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "is valid Url");
    return true;
}

#endif

void AppDomainVerifyMgrClient::UpdateWhiteListUrls(const std::vector<std::string>& urls)
{
#ifdef _CUT_LINK_CONVERT_
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "not support, will return!");
    return;
#else
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        appDomainVerifyMgrServiceProxy_->UpdateWhiteListUrls(urls);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
#endif
}

int AppDomainVerifyMgrClient::QueryAppDetailsWant(const std::string& link, AAFwk::Want& want)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        return appDomainVerifyMgrServiceProxy_->QueryAppDetailsWant(link, want);
        IPCSkeleton::SetCallingIdentity(identity);
    }
    return ErrorCode::E_INTERNAL_ERR;
}

int AppDomainVerifyMgrClient::QueryAssociatedDomains(const std::string& bundleName, std::vector<std::string>& domains)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        return appDomainVerifyMgrServiceProxy_->QueryAssociatedDomains(bundleName, domains);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return ErrorCode::E_INTERNAL_ERR;
}
int AppDomainVerifyMgrClient::QueryAssociatedBundleNames(
    const std::string& domain, std::vector<std::string>& bundleNames)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        return appDomainVerifyMgrServiceProxy_->QueryAssociatedBundleNames(domain, bundleNames);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return ErrorCode::E_INTERNAL_ERR;
}
int AppDomainVerifyMgrClient::GetDeferredLink(std::string& link)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        return appDomainVerifyMgrServiceProxy_->GetDeferredLink(link);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return ErrorCode::E_INTERNAL_ERR;
}
bool AppDomainVerifyMgrClient::IsShortUrl(const std::string& url)
{
#ifdef _CUT_LINK_CONVERT_
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "not support, will return false!");
    return false;
#else
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called, url %{public}s", MaskStr(url).c_str());
    if (!IsValidUrl(url)) {
        return false;
    }
    bool ret{ false };
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        ret = appDomainVerifyMgrServiceProxy_->IsShortUrl(url);
        IPCSkeleton::SetCallingIdentity(identity);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end, IsAtomicServiceUrl:%{public}d", ret);
    return ret;
#endif
}

void AppDomainVerifyMgrClient::ConvertFromShortUrl(OHOS::AAFwk::Want& originWant, sptr<IConvertCallback>& callback)
{
#ifdef _CUT_LINK_CONVERT_
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "not support, will return false!");
    if (callback) {
        callback->OnConvert(-1, implicitWant);
    }
#else
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (IsServiceAvailable()) {
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        appDomainVerifyMgrServiceProxy_->ConvertFromShortUrl(originWant, callback);
        IPCSkeleton::SetCallingIdentity(identity);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
#endif
}
AppDomainVerifyMgrSaDeathRecipient::AppDomainVerifyMgrSaDeathRecipient()
{
}

AppDomainVerifyMgrSaDeathRecipient::~AppDomainVerifyMgrSaDeathRecipient()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "deathRecipient dead.");
}

void AppDomainVerifyMgrSaDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& object)
{
    APP_DOMAIN_VERIFY_HILOGD(
        APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "AppDomainVerifyMgrSaDeathRecipient on remote systemAbility died.");
    AppDomainVerifyMgrClient::GetInstance()->OnRemoteSaDied(object);
}

}  // namespace AppDomainVerify
}  // namespace OHOS
