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

#include "target_info.h"
#include "app_domain_verify_hilog.h"
#include "app_domain_verify_parcel_util.h"
namespace OHOS::AppDomainVerify {

bool TargetInfo::Marshalling(Parcel& parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, targetType);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &targetWant);
    return true;
}
TargetInfo* TargetInfo::Unmarshalling(Parcel& parcel)
{
    auto* targetInfo = new (std::nothrow) TargetInfo();
    if ((targetInfo != nullptr) && (!targetInfo->ReadFromParcel(parcel))) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to read from parcel");
        delete targetInfo;
        targetInfo = nullptr;
    }
    return targetInfo;
}
bool TargetInfo::ReadFromParcel(Parcel& parcel)
{
    uint32_t type = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, type);
    targetType = static_cast<TargetType>(type);
    std::unique_ptr<OHOS::AAFwk::Want> w(parcel.ReadParcelable<OHOS::AAFwk::Want>());
    OHOS::AAFwk::Want want;
    if (!w) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable want failed.");
        return ERR_INVALID_VALUE;
    }
    targetWant = *w;
    return true;
}
}