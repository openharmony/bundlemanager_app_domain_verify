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
#ifndef APP_VERIFY_BASE_INFO_H
#define APP_VERIFY_BASE_INFO_H
#include <string>
#include "parcel.h"

namespace OHOS {
namespace AppDomainVerify {
struct AppVerifyBaseInfo : public Parcelable {
    std::string appIdentifier{};
    std::string bundleName{};
    std::string fingerprint{};
    AppVerifyBaseInfo();
    AppVerifyBaseInfo(const AppVerifyBaseInfo &appVerifyBaseInfo);

    static AppVerifyBaseInfo *Unmarshalling(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;

private:
    bool ReadFromParcel(Parcel &parcel);
};
}  // namespace AppDomainVerify
}  // namespace OHOS

#endif  // APP_VERIFY_BASE_INFO_H
