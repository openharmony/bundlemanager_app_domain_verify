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
#include "app_domain_verify_mgr_service_stub.h"
#include "app_domain_verify_mgr_interface_code.h"
#include "errors.h"
#include "parcel_util.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppDomainVerify {
namespace {
constexpr int32_t DELAY_TIME = 300000;  // 5min = 5*60*1000
const std::string TASK_ID = "unload";
}
AppDomainVerifyMgrServiceStub::AppDomainVerifyMgrServiceStub()
{
}
AppDomainVerifyMgrServiceStub::~AppDomainVerifyMgrServiceStub()
{
}
int32_t AppDomainVerifyMgrServiceStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "onRemoteRequest##code = %{public}u", code);
    std::u16string myDescripter = AppDomainVerifyMgrServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "end##descriptor checked fail");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    switch (code) {
        case static_cast<uint32_t>(static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::QUERY_VERIFY_STATUS)):
            return OnQueryDomainVerifyStatus(data, reply);
        case static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN):
            return OnVerifyDomain(data, reply);
        case static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::CLEAR_DOMAIN_VERIFY_RESULT):
            return OnClearDomainVerifyStatus(data, reply);
        case static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES):
            return OnFilterAbilities(data, reply);
        case static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::QUERY_ALL_VERIFY_STATUS):
            return OnQueryAllDomainVerifyStatus(data, reply);
        case static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::SAVE_VERIFY_STATUS):
            return OnSaveDomainVerifyStatus(data, reply);
        case static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::IS_ATOMIC_SERVICE_URL):
            return OnIsAtomicServiceUrl(data, reply);
        case static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::CONVERT_TO_EXPLICIT_WANT):
            return OnConvertToExplicitWant(data, reply);
        case static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::UPDATE_WHITE_LIST_URLS):
            return OnUpdateWhiteListUrls(data, reply);
        default:
            APP_DOMAIN_VERIFY_HILOGW(
                APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "receive unknown code, code = %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int32_t AppDomainVerifyMgrServiceStub::OnVerifyDomain(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::string appIdentifier = data.ReadString();
    std::string bundleName = data.ReadString();
    std::string fingerprint = data.ReadString();
    int32_t size = data.ReadInt32();
    if (IsInvalidParcelArraySize(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable size invalid.");
        return false;
    }
    std::vector<SkillUri> skillUris;
    for (int32_t i = 0; i < size; i++) {
        std::unique_ptr<SkillUri> info(data.ReadParcelable<SkillUri>());
        if (!info) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable skillUri failed.");
            return ERR_INVALID_VALUE;
        }
        skillUris.emplace_back(*info);
    }

    VerifyDomain(appIdentifier, bundleName, fingerprint, skillUris);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}

int32_t AppDomainVerifyMgrServiceStub::OnClearDomainVerifyStatus(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::string appIdentifier = data.ReadString();
    std::string bundleName = data.ReadString();
    bool status = ClearDomainVerifyStatus(appIdentifier, bundleName);

    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Bool, reply, status);
    return ERR_OK;
}

int32_t AppDomainVerifyMgrServiceStub::OnFilterAbilities(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::unique_ptr<OHOS::AAFwk::Want> w(data.ReadParcelable<OHOS::AAFwk::Want>());
    OHOS::AAFwk::Want want;
    if (!w) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable want failed.");
        return ERR_INVALID_VALUE;
    }
    want = *w;
    std::vector<OHOS::AppExecFwk::AbilityInfo> originAbilityInfos;
    int32_t size = data.ReadInt32();
    if (IsInvalidParcelArraySize(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable size invalid.");
        return false;
    }
    for (int32_t i = 0; i < size; i++) {
        std::unique_ptr<OHOS::AppExecFwk::AbilityInfo> info(data.ReadParcelable<OHOS::AppExecFwk::AbilityInfo>());
        if (!info) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable abilityInfo failed.");
            return ERR_INVALID_VALUE;
        }
        originAbilityInfos.emplace_back(*info);
    }
    std::vector<OHOS::AppExecFwk::AbilityInfo> filtedAbilityInfos;
    bool status = FilterAbilities(want, originAbilityInfos, filtedAbilityInfos);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Bool, reply, status);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Int32, reply, filtedAbilityInfos.size());
    for (auto& it : filtedAbilityInfos) {
        WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Parcelable, reply, &it);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}

int32_t AppDomainVerifyMgrServiceStub::OnQueryDomainVerifyStatus(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::string bundleName = data.ReadString();
    DomainVerifyStatus domainVerificationState;
    bool status = QueryDomainVerifyStatus(bundleName, domainVerificationState);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Bool, reply, status);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Int32, reply, domainVerificationState);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}

int32_t AppDomainVerifyMgrServiceStub::OnQueryAllDomainVerifyStatus(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);

    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    bool status = QueryAllDomainVerifyStatus(bundleVerifyStatusInfo);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Bool, reply, status);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Parcelable, reply, &bundleVerifyStatusInfo);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}

int32_t AppDomainVerifyMgrServiceStub::OnSaveDomainVerifyStatus(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::string bundleName = data.ReadString();
    std::unique_ptr<VerifyResultInfo> verifyResultInfo(data.ReadParcelable<VerifyResultInfo>());
    if (verifyResultInfo == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "verifyResultInfo null");
        return ERR_INVALID_VALUE;
    }
    bool status = SaveDomainVerifyStatus(bundleName, *verifyResultInfo);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Bool, reply, status);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}
int32_t AppDomainVerifyMgrServiceStub::OnIsAtomicServiceUrl(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::string url = data.ReadString();
    bool status = IsAtomicServiceUrl(url);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Bool, reply, status);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}
int32_t AppDomainVerifyMgrServiceStub::OnUpdateWhiteListUrls(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    uint32_t size = data.ReadUint32();
    std::vector<std::string> urls;
    for (uint32_t i = 0; i < size; i++) {
        auto url = data.ReadString();
        urls.emplace_back(url);
    }
    UpdateWhiteListUrls(urls);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return 0;
}
int32_t AppDomainVerifyMgrServiceStub::OnConvertToExplicitWant(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    OHOS::AAFwk::Want want;
    std::unique_ptr<OHOS::AAFwk::Want> w(data.ReadParcelable<OHOS::AAFwk::Want>());
    if (!w) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable want failed.");
        return ERR_INVALID_VALUE;
    }
    want = *w;
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    if (object == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IConvertCallback> cleanCacheCallback = iface_cast<IConvertCallback>(object);
    ConvertToExplicitWant(want, cleanCacheCallback);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}
}  // namespace AppDomainVerify
}  // namespace OHOS