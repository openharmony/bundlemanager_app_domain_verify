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

#include "app_verify_base_info.h"
#include "parcel_util.h"

namespace OHOS {
namespace AppDomainVerify {
AppVerifyBaseInfo::AppVerifyBaseInfo()
{
    appIdentifier = "";
    bundleName = "";
    fingerprint = "";
}

AppVerifyBaseInfo::AppVerifyBaseInfo(const AppVerifyBaseInfo &appVerifyBaseInfo)
{
    appIdentifier = appVerifyBaseInfo.appIdentifier;
    bundleName = appVerifyBaseInfo.bundleName;
    fingerprint = appVerifyBaseInfo.fingerprint;
}
bool AppVerifyBaseInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, appIdentifier);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fingerprint);
    return true;
}
bool AppVerifyBaseInfo::ReadFromParcel(Parcel &parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, appIdentifier);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, fingerprint);
    return true;
}

AppVerifyBaseInfo *AppVerifyBaseInfo::Unmarshalling(Parcel &parcel)
{
    AppVerifyBaseInfo *appVerifyBaseInfo = new (std::nothrow) AppVerifyBaseInfo();
    if (appVerifyBaseInfo && !appVerifyBaseInfo->ReadFromParcel(parcel)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "read from parcel failed");
        delete appVerifyBaseInfo;
        appVerifyBaseInfo = nullptr;
    }
    return appVerifyBaseInfo;
}

}  // namespace AppDomainVerify
}  // namespace OHOS
