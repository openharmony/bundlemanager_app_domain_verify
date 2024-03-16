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
#include "skill_uri.h"
#include "parcel.h"
#include "parcel_util.h"

namespace OHOS {
namespace AppDomainVerify {

bool SkillUri::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, scheme);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, host);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, port);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, path);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, pathStartWith);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, pathRegex);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, type);
    return true;
}
bool SkillUri::ReadFromParcel(Parcel &parcel)
{
    if (parcel.ReadString(scheme) && parcel.ReadString(host)) {
        // todo 判断host是否要string16
        port = parcel.ReadString();
        path = parcel.ReadString();
        pathStartWith = parcel.ReadString();
        pathRegex = parcel.ReadString();
        type = parcel.ReadString();
        return true;
    }
    return false;
}

SkillUri *SkillUri::Unmarshalling(Parcel &parcel)
{
    SkillUri *skillUri = new (std::nothrow) SkillUri();
    if (skillUri && !skillUri->ReadFromParcel(parcel)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "read from parcel failed");
        delete skillUri;
        skillUri = nullptr;
    }
    return skillUri;
}
}  // namespace AppDomainVerify
}  // namespace OHOS