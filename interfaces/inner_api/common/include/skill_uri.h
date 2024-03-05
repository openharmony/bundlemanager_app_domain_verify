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
#ifndef APP_DOMAIN_VERIFY_SKILL_UTI_H
#define APP_DOMAIN_VERIFY_SKILL_UTI_H
#include <string>

#include "parcel.h"
namespace OHOS {
namespace AppDomainVerify {
struct SkillUri : public Parcelable {
public:
    static SkillUri *Unmarshalling(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;
    bool ReadFromParcel(Parcel &parcel);

    std::string scheme;
    std::string host;
    std::string port;
    std::string path;
    std::string pathStartWith;
    std::string pathRegex;
    std::string type;
};
}  // namespace AppDomainVerify
}  // namespace OHOS
#endif