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
#include "app_domain_verify_agent_service_stub.h"
#include "agent_interface_code.h"
#include "errors.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "app_domain_verify_parcel_util.h"

namespace OHOS {
namespace AppDomainVerify {

int32_t AppDomainVerifyAgentServiceStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "onRemoteRequest##code = %{public}u", code);
    std::u16string myDescripter = AppDomainVerifyAgentServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "end##descriptor checked fail");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    switch (code) {
        case static_cast<uint32_t>(static_cast<uint32_t>(AgentInterfaceCode::SINGLE_VERIFY)):
            return OnSingleVerify(data, reply);
        case static_cast<uint32_t>(AgentInterfaceCode::CONVERT_TO_EXPLICIT_WANT):
            return OnConvertToExplicitWant(data, reply);
        case static_cast<uint32_t>(AgentInterfaceCode::COMMON_TRANSACT):
            return OnCommonTransact(data, reply);
        default:
            APP_DOMAIN_VERIFY_HILOGW(
                APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "receive unknown code, code = %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int32_t AppDomainVerifyAgentServiceStub::OnSingleVerify(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    std::unique_ptr<AppVerifyBaseInfo> info(data.ReadParcelable<AppVerifyBaseInfo>());
    if (!info) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "read parcelable AppVerifyBaseInfo failed.");
        return ERR_INVALID_VALUE;
    }
    AppVerifyBaseInfo appVerifyBaseInfo = *info;

    std::unique_ptr<VerifyResultInfo> result(data.ReadParcelable<VerifyResultInfo>());
    if (!result) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "read parcelable VerifyResultInfo failed.");
        return ERR_INVALID_VALUE;
    }
    VerifyResultInfo verifyBaseInfo = *result;

    SingleVerify(appVerifyBaseInfo, verifyBaseInfo);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "call end");
    return ERR_OK;
}
int32_t AppDomainVerifyAgentServiceStub::OnConvertToExplicitWant(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
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
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return ERR_OK;
}
int32_t AppDomainVerifyAgentServiceStub::OnCommonTransact(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    OHOS::AAFwk::Want want;
    std::string request;
    uint32_t opcode;
    READ_PARCEL_AND_RETURN_INT_IF_FAIL(Uint32, data, opcode);
    READ_PARCEL_AND_RETURN_INT_IF_FAIL(String, data, request);
    std::string response;
    auto status = CommonTransact(opcode, request, response);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(Int32, reply, status);
    WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(String, reply, response);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return ERR_OK;
    return 0;
}

}  // namespace AppDomainVerify
}  // namespace OHOS