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
constexpr int32_t DELAY_TIME = 300000; // 5min = 5*60*1000
const std::string TASK_ID = "unload";
}
AppDomainVerifyMgrServiceStub::AppDomainVerifyMgrServiceStub()
{
    memberFuncMap_[static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::QUERY_VERIFY_STATUS)] =
        &AppDomainVerifyMgrServiceStub::OnQueryDomainVerifyStatus;
    memberFuncMap_[static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN)] =
        &AppDomainVerifyMgrServiceStub::OnVerifyDomain;
    memberFuncMap_[static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::CLEAR_DOMAIN_VERIFY_RESULT)] =
        &AppDomainVerifyMgrServiceStub::OnClearDomainVerifyStatus;
    memberFuncMap_[static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES)] =
        &AppDomainVerifyMgrServiceStub::OnFilterAbilities;
    memberFuncMap_[static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::QUERY_ALL_VERIFY_STATUS)] =
        &AppDomainVerifyMgrServiceStub::OnQueryAllDomainVerifyStatus;
    memberFuncMap_[static_cast<uint32_t>(AppDomainVerifyMgrInterfaceCode::SAVE_VERIFY_STATUS)] =
        &AppDomainVerifyMgrServiceStub::OnSaveDomainVerifyStatus;
}
AppDomainVerifyMgrServiceStub::~AppDomainVerifyMgrServiceStub()
{
    memberFuncMap_.clear();
}
int32_t AppDomainVerifyMgrServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "onRemoteRequest##code = %{public}u", code);
    PostDelayUnloadTask();
    std::u16string myDescripter = AppDomainVerifyMgrServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "end##descriptor checked fail");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    int ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "end##ret = %{public}d", ret);
    return ret;
}

int32_t AppDomainVerifyMgrServiceStub::OnVerifyDomain(MessageParcel &data, MessageParcel &reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::string appIdentifier = data.ReadString();
    std::string bundleName = data.ReadString();
    std::string fingerprint = data.ReadString();
    int32_t size = data.ReadInt32();
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

int32_t AppDomainVerifyMgrServiceStub::OnClearDomainVerifyStatus(MessageParcel &data, MessageParcel &reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::string appIdentifier = data.ReadString();
    std::string bundleName = data.ReadString();
    bool status = ClearDomainVerifyStatus(appIdentifier, bundleName);

    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Bool, reply, status);
    return ERR_OK;
}

int32_t AppDomainVerifyMgrServiceStub::OnFilterAbilities(MessageParcel &data, MessageParcel &reply)
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
    for (auto &it : filtedAbilityInfos) {
        WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Parcelable, reply, &it);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}

int32_t AppDomainVerifyMgrServiceStub::OnQueryDomainVerifyStatus(MessageParcel &data, MessageParcel &reply)
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

int32_t AppDomainVerifyMgrServiceStub::OnQueryAllDomainVerifyStatus(MessageParcel &data, MessageParcel &reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);

    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    bool status = QueryAllDomainVerifyStatus(bundleVerifyStatusInfo);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Bool, reply, status);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Parcelable, reply, &bundleVerifyStatusInfo);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}

int32_t AppDomainVerifyMgrServiceStub::OnSaveDomainVerifyStatus(MessageParcel &data, MessageParcel &reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::string bundleName = data.ReadString();
    std::unique_ptr<VerifyResultInfo> verifyResultInfo(data.ReadParcelable<VerifyResultInfo>());
    bool status = SaveDomainVerifyStatus(bundleName, *verifyResultInfo);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Bool, reply, status);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}

void AppDomainVerifyMgrServiceStub::PostDelayUnloadTask()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    if (runner_ == nullptr) {
        runner_ = AppExecFwk::EventRunner::Create("unload");
    }
    if (runner_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "runner init failed!");
        return;
    }
    if (unloadHandler_ == nullptr) {
        unloadHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    }
    if (unloadHandler_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "unloadHandler init failed!");
        return;
    }

    auto task = [this]() {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "do unload task");
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy == nullptr) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get samgr failed");
            return;
        }
        int32_t ret = samgrProxy->UnloadSystemAbility(APP_DOMAIN_VERIFY_MANAGER_SA_ID);
        if (ret != 0) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "remove system ability failed");
            return;
        }
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "do unload task done");
    };
    unloadHandler_->RemoveTask(TASK_ID);
    unloadHandler_->PostTask(task, TASK_ID, DELAY_TIME);
}
}  // namespace AppDomainVerify
}  // namespace OHOS