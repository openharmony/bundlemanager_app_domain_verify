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
#include "parcel_util.h"

namespace OHOS {
namespace AppDomainVerify {
AppDomainVerifyMgrServiceProxy::AppDomainVerifyMgrServiceProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IAppDomainVerifyMgrService>(object)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "new instance created.");
}
AppDomainVerifyMgrServiceProxy::~AppDomainVerifyMgrServiceProxy()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "instance dead.");
}
void AppDomainVerifyMgrServiceProxy::VerifyDomain(const std::string &appIdentifier, const std::string &bundleName,
    const std::string &fingerprint, const std::vector<SkillUri> &skillUris)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
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
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
}

bool AppDomainVerifyMgrServiceProxy::ClearDomainVerifyStatus(const std::string &appIdentifier,
    const std::string &bundleName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, data, appIdentifier);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, data, bundleName);
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::CLEAR_DOMAIN_VERIFY_RESULT, data, reply,
        option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "ClearDomainVerifyStatus failed, error: %d",
            error);
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return reply.ReadBool();
}

bool AppDomainVerifyMgrServiceProxy::FilterAbilities(const OHOS::AAFwk::Want &want,
    const std::vector<OHOS::AppExecFwk::AbilityInfo> &originAbilityInfos,
    std::vector<OHOS::AppExecFwk::AbilityInfo> &filtedAbilityInfos)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
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
    if (!reply.ReadBool()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "FilterAbilities failed");
        return false;
    }
    int32_t infoSize = reply.ReadInt32();
    for (int32_t i = 0; i < infoSize; i++) {
        std::unique_ptr<OHOS::AppExecFwk::AbilityInfo> info(reply.ReadParcelable<OHOS::AppExecFwk::AbilityInfo>());
        if (info == nullptr) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Read Parcelable AbilityInfo failed");
            return false;
        }
        filtedAbilityInfos.emplace_back(*info);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyMgrServiceProxy::QueryDomainVerifyStatus(const std::string &bundleName,
    DomainVerifyStatus &domainVerificationState)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, data, bundleName);

    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::QUERY_VERIFY_STATUS, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryDomainVerifyStatus failed, error: %d",
            error);
        return false;
    }
    if (!reply.ReadBool()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryDomainVerifyStatus failed");
        return false;
    }

    domainVerificationState = static_cast<DomainVerifyStatus>(reply.ReadInt32());
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyMgrServiceProxy::QueryAllDomainVerifyStatus(BundleVerifyStatusInfo &bundleVerifyStatusInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::QUERY_ALL_VERIFY_STATUS, data, reply,
        option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryAllDomainVerifyStatus failed, error: %d",
            error);
        return false;
    }
    if (!reply.ReadBool()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryAllDomainVerifyStatus failed");
        return false;
    }
    std::unique_ptr<BundleVerifyStatusInfo> info(reply.ReadParcelable<BundleVerifyStatusInfo>());
    if (info == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "Read Parcelable BundleVerifyStatusInfo failed");
        return false;
    }
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_ = info->bundleVerifyStatusInfoMap_;
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyMgrServiceProxy::SaveDomainVerifyStatus(const std::string &bundleName,
    const VerifyResultInfo &verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s called", __func__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, data, bundleName);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, data, &verifyResultInfo);
    int32_t error = Remote()->SendRequest(AppDomainVerifyMgrInterfaceCode::SAVE_VERIFY_STATUS, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "QueryAllDomainVerifyStatus failed, error: %d",
            error);
        return false;
    }
    if (!reply.ReadBool()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "SaveDomainVerifyStatus failed");
        return false;
    }

    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "%s call end", __func__);
    return true;
}
}  // namespace AppDomainVerify
}  // namespace OHOS
