/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "zidl/convert_callback_stub.h"
#include "zidl/convert_callback_interface_code.h"
#include "app_domain_verify_hilog.h"
#include "app_domain_verify_parcel_util.h"

namespace OHOS {
namespace AppDomainVerify {
ConvertCallbackStub::ConvertCallbackStub()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "new instance created.");
}

ConvertCallbackStub::~ConvertCallbackStub()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "instance dead.");
}

int ConvertCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT,
        "ConvertCallbackStub callback host onReceived message, the message code is %{public}u", code);
    std::u16string descripter = ConvertCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "fail to write reply message in convert due to the reply is nullptr");
        return OBJECT_NULL;
    }

    switch (code) {
        case static_cast<uint32_t>(ConvertCallbackInterfaceCode::ON_CONVERT_CALLBACK): {
            int32_t retCode = data.ReadInt32();
            std::unique_ptr<OHOS::AAFwk::Want> w(data.ReadParcelable<OHOS::AAFwk::Want>());
            OHOS::AAFwk::Want want;
            if (!w) {
                APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable want failed.");
                return ERR_INVALID_VALUE;
            }
            want = *w;
            OnConvert(retCode, want);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}
}  // namespace AppExecFwk
}  // namespace OHOS
