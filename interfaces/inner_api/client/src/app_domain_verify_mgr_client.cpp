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

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "app_domain_verify_mgr_client.h"
#include "system_ability_definition.h"
#include "app_domain_verify_hisysevent.h"
#include "zidl/convert_callback_stub.h"
#include "regex.h"

namespace OHOS {
namespace AppDomainVerify {
std::mutex AppDomainVerifyMgrClient::proxyLock_;
sptr<IAppDomainVerifyMgrService> AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_;
AppDomainVerifyMgrClient::StaticDestoryMonitor AppDomainVerifyMgrClient::staticDestoryMonitor_;
#ifndef _CUT_LINK_CONVERT_
static const std::string SCHEME_HTTPS("https");
static const char* PATTEN = "^[a-zA-Z0-9_-]{1,99}$";
constexpr int REG_ERR_BUF = 1024;
constexpr int NM = 10;
#endif
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
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    INSTALL_EVENT(appIdentifier, bundleName);
    if (IsServiceAvailable()) {
        appDomainVerifyMgrServiceProxy_->VerifyDomain(appIdentifier, bundleName, fingerprint, skillUris);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
}

bool AppDomainVerifyMgrClient::ClearDomainVerifyStatus(const std::string& appIdentifier, const std::string& bundleName)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    UNINSTALL_EVENT(appIdentifier, bundleName);
    bool clearResult = false;
    if (IsServiceAvailable()) {
        clearResult = appDomainVerifyMgrServiceProxy_->ClearDomainVerifyStatus(appIdentifier, bundleName);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return clearResult;
}

bool AppDomainVerifyMgrClient::FilterAbilities(const OHOS::AAFwk::Want& want,
    const std::vector<OHOS::AppExecFwk::AbilityInfo>& originAbilityInfos,
    std::vector<OHOS::AppExecFwk::AbilityInfo>& filtedAbilityInfos)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    bool filterSuccess = false;
    if (IsServiceAvailable()) {
        filterSuccess = appDomainVerifyMgrServiceProxy_->FilterAbilities(want, originAbilityInfos, filtedAbilityInfos);
    }

    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end, filted ability size:%{public}zu",
        __func__, filtedAbilityInfos.size());
    return filterSuccess;
}

bool AppDomainVerifyMgrClient::QueryDomainVerifyStatus(
    const std::string& bundleName, DomainVerifyStatus& domainVerificationState)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    bool querySuccess = false;
    if (IsServiceAvailable()) {
        querySuccess = appDomainVerifyMgrServiceProxy_->QueryDomainVerifyStatus(bundleName, domainVerificationState);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return querySuccess;
}

bool AppDomainVerifyMgrClient::QueryAllDomainVerifyStatus(BundleVerifyStatusInfo& bundleVerifyStatusInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    bool querySuccess = false;
    if (IsServiceAvailable()) {
        querySuccess = appDomainVerifyMgrServiceProxy_->QueryAllDomainVerifyStatus(bundleVerifyStatusInfo);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return querySuccess;
}

bool AppDomainVerifyMgrClient::SaveDomainVerifyStatus(
    const std::string& bundleName, const VerifyResultInfo& verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    bool saveSuccess = false;
    if (IsServiceAvailable()) {
        saveSuccess = appDomainVerifyMgrServiceProxy_->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return saveSuccess;
}

bool AppDomainVerifyMgrClient::IsServiceAvailable()
{
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (appDomainVerifyMgrServiceProxy_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Redo ConnectService");
        ConnectService();
    }

    if (appDomainVerifyMgrServiceProxy_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Service proxy null.");
        return false;
    }
    return true;
}
void AppDomainVerifyMgrClient::ConnectService()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "ConnectService start.");
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Get SystemAbilityManager failed.");
        appDomainVerifyMgrServiceProxy_ = nullptr;
        return;
    }
    sptr<IRemoteObject> remoteObject = samgrProxy->CheckSystemAbility(APP_DOMAIN_VERIFY_MANAGER_SA_ID);
    if (remoteObject != nullptr) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Get AppDomainVerifyMgrServiceProxy succeed.");
        if (deathRecipient_ == nullptr) {
            deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new AppDomainVerifyMgrSaDeathRecipient());
        }
        remoteObject->AddDeathRecipient(deathRecipient_);
        appDomainVerifyMgrServiceProxy_ = iface_cast<IAppDomainVerifyMgrService>(remoteObject);
        return;
    }
    APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Getting AppDomainVerifyMgrServiceProxy failed.");
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
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s not support, will return false.!", __func__);
    if (callback) {
        callback->OnConvert(-1, implicitWant);
    }
#else
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    if (IsServiceAvailable()) {
        appDomainVerifyMgrServiceProxy_->ConvertToExplicitWant(implicitWant, callback);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
#endif
}
#ifndef _CUT_LINK_CONVERT_
bool AppDomainVerifyMgrClient::IsValidPath(const std::string& path)
{
    const char* bematch = path.c_str();
    char errbuf[REG_ERR_BUF];
    regex_t reg;
    int errNum = 0;
    int nm = NM;
    regmatch_t pmatch[nm];
    if (regcomp(&reg, PATTEN, REG_EXTENDED) < 0) {
        regerror(errNum, &reg, errbuf, sizeof(errbuf));
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "regexec error:%{public}s", errbuf);
        return false;
    }
    errNum = regexec(&reg, bematch, nm, pmatch, 0);
    if (errNum == REG_NOMATCH) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "regexec no match");
        return false;
    } else if (errNum) {
        regerror(errNum, &reg, errbuf, sizeof(errbuf));
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "regexec error:%{public}s", errbuf);
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "is valid path");
    return true;
}
bool AppDomainVerifyMgrClient::IsValidUrl(OHOS::Uri& uri)
{
    if (uri.GetScheme() != SCHEME_HTTPS) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "scheme:%{public}s is not https", uri.GetScheme().c_str());
        return false;
    }
    if (uri.GetHost().empty()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "host is empty");
        return false;
    }
    std::vector<std::string> segments;
    uri.GetPathSegments(segments);
    if (segments.size() != 1) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "short path:%{public}s is more than one", uri.GetPath().c_str());
        return false;
    }
    if (!IsValidPath(segments[0])) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT,
            "short path:%{public}s must only contains number,alphabet or dash line!", segments[0].c_str());
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "is valid Url");
    return true;
}

#endif
bool AppDomainVerifyMgrClient::IsAtomicServiceUrl(const std::string& url)
{
#ifdef _CUT_LINK_CONVERT_
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s not support, will return false.!", __func__);
    return false;
#else
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    Uri uri(url);
    if (!IsValidUrl(uri)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "url %{public}s is invalid!", url.c_str());
        return false;
    }
    bool ret{ false };
    if (IsServiceAvailable()) {
        ret = appDomainVerifyMgrServiceProxy_->IsAtomicServiceUrl(uri.GetScheme() + "://" + uri.GetHost());
    }
    APP_DOMAIN_VERIFY_HILOGI(
        APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end, IsAtomicServiceUrl:%{public}d", __func__, ret);
    return ret;
#endif
}
void AppDomainVerifyMgrClient::UpdateWhiteListUrls(const std::vector<std::string>& urls)
{
#ifdef _CUT_LINK_CONVERT_
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s not support, will return.!", __func__);
    return;
#else
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    if (IsServiceAvailable()) {
        appDomainVerifyMgrServiceProxy_->UpdateWhiteListUrls(urls);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
#endif
}
int AppDomainVerifyMgrClient::QueryAssociatedDomains(
    const std::string& bundleName, std::vector<std::string>& domains)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    if (IsServiceAvailable()) {
        return appDomainVerifyMgrServiceProxy_->QueryAssociatedDomains(bundleName, domains);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return false;
}
int AppDomainVerifyMgrClient::QueryAssociatedBundleNames(
    const std::string& domain, std::vector<std::string>& bundleNames)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    if (IsServiceAvailable()) {
        return appDomainVerifyMgrServiceProxy_->QueryAssociatedBundleNames(domain, bundleNames);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return false;
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
