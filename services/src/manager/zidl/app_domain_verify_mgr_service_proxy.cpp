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
#include "app_domain_verify_mgr_service_proxy.h"
#include "app_domain_verify_mgr_interface_code.h"
#include "system_ability_definition.h"
#include "app_domain_verify_parcel_util.h"
#include "want.h"

namespace OHOS {
namespace AppDomainVerify {
AppDomainVerifyMgrServiceProxy::AppDomainVerifyMgrServiceProxy(const sptr<IRemoteObject>& object)
    : IRemoteProxy<IAppDomainVerifyMgrService>(object)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "new instance created.");
}
AppDomainVerifyMgrServiceProxy::~AppDomainVerifyMgrServiceProxy()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "instance dead.");
}
void AppDomainVerifyMgrServiceProxy::VerifyDomain(const std::string& appIdentifier, const std::string& bundleName,
    const std::string& fingerprint, const std::vector<SkillUri>& skillUris)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, appIdentifier);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, fingerprint);

    uint32_t size = static_cast<uint32_t>(skillUris.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, size);
    for (uint32_t i = 0; i < skillUris.size(); ++i) {
        WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &skillUris[i]);
    }
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "VerifyDomain failed, error: %{public}d", error);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
}

bool AppDomainVerifyMgrServiceProxy::ClearDomainVerifyStatus(
    const std::string& appIdentifier, const std::string& bundleName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, data, appIdentifier);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, data, bundleName);
    int32_t error = Remote()->SendRequest(
        AppDomainVerifyMgrInterfaceCode::CLEAR_DOMAIN_VERIFY_RESULT, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "ClearDomainVerifyStatus failed, error: %d", error);
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return reply.ReadBool();
}

bool AppDomainVerifyMgrServiceProxy::FilterAbilities(const OHOS::AAFwk::Want& want,
    const std::vector<OHOS::AppExecFwk::AbilityInfo>& originAbilityInfos,
    std::vector<OHOS::AppExecFwk::AbilityInfo>& filteredAbilityInfos)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, data, &want);
    uint32_t originAbilityInfoSize = static_cast<uint32_t>(originAbilityInfos.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, data, originAbilityInfoSize);

    for (uint32_t i = 0; i < originAbilityInfos.size(); ++i) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, data, &originAbilityInfos[i]);
    }
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "FilterAbilities failed, error: %d", error);
        return false;
    }
    bool status = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, reply, status);
    if (!status) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "FilterAbilities status failed, error: %d", error);
        return false;
    }
    int32_t infoSize = reply.ReadInt32();
    if (IsInvalidParcelArraySize(infoSize)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable size invalid.");
        return false;
    }
    for (int32_t i = 0; i < infoSize; i++) {
        std::unique_ptr<OHOS::AppExecFwk::AbilityInfo> info(reply.ReadParcelable<OHOS::AppExecFwk::AbilityInfo>());
        if (info == nullptr) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Read Parcelable AbilityInfo failed");
            return false;
        }
        filteredAbilityInfos.emplace_back(*info);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return true;
}

bool AppDomainVerifyMgrServiceProxy::QueryDomainVerifyStatus(
    const std::string& bundleName, DomainVerifyStatus& domainVerificationState)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, data, bundleName);

    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::QUERY_VERIFY_STATUS, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryDomainVerifyStatus failed, error: %d", error);
        return false;
    }
    bool status = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, reply, status);
    if (!status) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryDomainVerifyStatus status failed, error: %d", error);
        return false;
    }
    domainVerificationState = static_cast<DomainVerifyStatus>(reply.ReadInt32());
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return true;
}

bool AppDomainVerifyMgrServiceProxy::QueryAllDomainVerifyStatus(BundleVerifyStatusInfo& bundleVerifyStatusInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    int32_t error = Remote()->SendRequest(
        AppDomainVerifyMgrInterfaceCode::QUERY_ALL_VERIFY_STATUS, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryAllDomainVerifyStatus failed, error: %d", error);
        return false;
    }
    bool status = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, reply, status);
    if (!status) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryAllDomainVerifyStatus status failed, error: %d", error);
        return false;
    }
    std::unique_ptr<BundleVerifyStatusInfo> info(reply.ReadParcelable<BundleVerifyStatusInfo>());
    if (info == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Read Parcelable BundleVerifyStatusInfo failed");
        return false;
    }
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_ = info->bundleVerifyStatusInfoMap_;

    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return true;
}

bool AppDomainVerifyMgrServiceProxy::SaveDomainVerifyStatus(
    const std::string& bundleName, const VerifyResultInfo& verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, data, bundleName);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, data, &verifyResultInfo);
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::SAVE_VERIFY_STATUS, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "SaveDomainVerifyStatus failed, error: %d", error);
        return false;
    }
    bool status = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, reply, status);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return status;
}
bool AppDomainVerifyMgrServiceProxy::IsAtomicServiceUrl(const std::string& url)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, data, url);
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::IS_ATOMIC_SERVICE_URL, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "IsAtomicServiceUrl failed, error: %d", error);
        return false;
    }
    bool status = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, reply, status);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return status;
}
void AppDomainVerifyMgrServiceProxy::UpdateWhiteListUrls(const std::vector<std::string>& urls)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, urls.size());
    for (const auto& url : urls) {
        WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, url);
    }
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::UPDATE_WHITE_LIST_URLS, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "IsAtomicServiceUrl failed, error: %d", error);
        return;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
}
void AppDomainVerifyMgrServiceProxy::ConvertToExplicitWant(
    OHOS::AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &implicitWant);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(RemoteObject, data, callback->AsObject());

    int32_t error = Remote()->SendRequest(
        AppDomainVerifyMgrInterfaceCode::CONVERT_TO_EXPLICIT_WANT, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "ConvertToExplicitWant failed, error: %d", error);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
}
int AppDomainVerifyMgrServiceProxy::QueryAssociatedDomains(
    const std::string& bundleName, std::vector<std::string>& domains)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(String, data, bundleName);

    int32_t error = Remote()->SendRequest(
        AppDomainVerifyMgrInterfaceCode::QUERY_ASSOCIATED_DOMAINS, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryAssociatedDomains failed, error: %d", error);
    }
    int32_t result = reply.ReadInt32();
    if (result != 0) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "result failed, result: %d", result);
        return result;
    }
    int32_t size = reply.ReadInt32();
    if (IsInvalidParcelArraySize(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable size invalid.");
        return ERR_INVALID_VALUE;
    }
    domains.clear();
    for (int32_t i = 0; i < size; i++) {
        std::string domain;
        READ_PARCEL_AND_RETURN_INT_IF_FAIL(String, reply, domain);
        domains.emplace_back(domain);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return result;
}
int AppDomainVerifyMgrServiceProxy::QueryAssociatedBundleNames(
    const std::string& domain, std::vector<std::string>& bundleNames)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(String, data, domain);

    int32_t error = Remote()->SendRequest(
        AppDomainVerifyMgrInterfaceCode::QUERY_ASSOCIATED_BUNDLE_NAMES, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryAssociatedBundleNames failed, error: %d", error);
    }
    int32_t result = reply.ReadInt32();
    if (result != 0) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "result failed, result: %d", result);
        return result;
    }
    int32_t size = reply.ReadInt32();
    if (IsInvalidParcelArraySize(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable size invalid.");
        return E_INTERNAL_ERR;
    }
    bundleNames.clear();
    for (int32_t i = 0; i < size; i++) {
        std::string bundleName;
        READ_PARCEL_AND_RETURN_INT_IF_FAIL(String, reply, bundleName);
        bundleNames.emplace_back(bundleName);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return result;
}
int AppDomainVerifyMgrServiceProxy::GetDeferredLink(std::string& link)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(InterfaceToken, data, GetDescriptor());
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::GET_DEFERRED_LINK, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "GetDeferredLink failed, error: %d", error);
    }
    int32_t result = reply.ReadInt32();
    if (result != 0) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "result failed, result: %d", result);
        return result;
    }
    READ_PARCEL_AND_RETURN_INT_IF_FAIL(String, reply, link);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return result;
}

int AppDomainVerifyMgrServiceProxy::QueryAppDetailsWant(const std::string& url, AAFwk::Want& want)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(String, data, url);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Parcelable, data, &want);
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::QUERY_APP_DETAILS_WANT, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryAppDetailsWant failed, error: %d", error);
    }
    int32_t result = reply.ReadInt32();
    if (result != 0) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "result failed, result: %d", result);
        return result;
    }
    std::unique_ptr<OHOS::AAFwk::Want> w(data.ReadParcelable<OHOS::AAFwk::Want>());
    if (!w) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable want failed.");
        return ERR_INVALID_VALUE;
    }
    want = *w;
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return result;
}
bool AppDomainVerifyMgrServiceProxy::IsShortUrl(const std::string& url)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, data, url);
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::IS_SHORT_URL, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "IsAtomicServiceUrl failed, error: %d", error);
        return false;
    }
    bool status = false;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, reply, status);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
    return status;
}
void AppDomainVerifyMgrServiceProxy::ConvertFromShortUrl(AAFwk::Want& originWant, sptr<IConvertCallback>& callback)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "called");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &originWant);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(RemoteObject, data, callback->AsObject());

    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::CONVERT_FROM_SHORT_URL, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "ConvertToExplicitWant failed, error: %d", error);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "call end");
}
}  // namespace AppDomainVerify
}  // namespace OHOS
