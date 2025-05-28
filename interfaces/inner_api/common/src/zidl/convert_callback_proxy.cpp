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
#include "zidl/convert_callback_proxy.h"
#include "zidl/convert_callback_interface_code.h"
#include "app_domain_verify_hilog.h"
#include "app_domain_verify_parcel_util.h"
#include "app_domain_verify_hisysevent.h"

namespace OHOS {
namespace AppDomainVerify {
ConvertCallbackProxy::ConvertCallbackProxy(const sptr<IRemoteObject>& object) : IRemoteProxy<IConvertCallback>(object)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "new instance created.");
}

ConvertCallbackProxy::~ConvertCallbackProxy()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "instance dead.");
}

void ConvertCallbackProxy::OnConvert(int resCode, OHOS::AAFwk::Want& want)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "OnConvert result %{public}d", resCode);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Int32, data, resCode);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &want);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "fail to call OnConvert, for Remote() is nullptr");
        return;
    }

    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(ConvertCallbackInterfaceCode::ON_CONVERT_CALLBACK), data, reply, option);
    if (ret != NO_ERROR) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT,
            "fail to call OnConvert, for transact is failed, error code is: %{public}d", ret);
        UNIVERSAL_ERROR_EVENT(CALL_BACK_FAULT);
    }
}
void ConvertCallbackProxy::OnConvert(int resCode, TargetInfo& targetInfo)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "OnConvert result %{public}d", resCode);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Int32, data, resCode);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &targetInfo);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT, "fail to call OnConvert, for Remote() is nullptr");
        return;
    }

    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(ConvertCallbackInterfaceCode::ON_CONVERT_CALLBACK), data, reply, option);
    if (ret != NO_ERROR) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT,
            "fail to call OnConvert, for transact is failed, error code is: %{public}d", ret);
        UNIVERSAL_ERROR_EVENT(CALL_BACK_FAULT);
    }
}
}
}