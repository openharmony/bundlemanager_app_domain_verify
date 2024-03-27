/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef APP_DOMAIN_BUNDLE_VERIFY_STATUS_INFO_H
#define APP_DOMAIN_BUNDLE_VERIFY_STATUS_INFO_H
#include <string>
#include "nlohmann/json.hpp"
#include "parcel.h"
#include "inner_verify_status.h"

namespace OHOS {
namespace AppDomainVerify {
using json = nlohmann::json;

struct VerifyResultInfo : public Parcelable {
public:
    bool Marshalling(Parcel &parcel) const override;
    static VerifyResultInfo *Unmarshalling(Parcel &parcel);

    static json VerifyResultInfoToJson(const VerifyResultInfo &verifyResultInfo);
    static VerifyResultInfo JsonToVerifyResultInfo(const json &verifyResultInfoJson);

private:
    bool ReadFromParcel(Parcel &parcel);

public:
    std::string appIdentifier;
    std::unordered_map<std::string, InnerVerifyStatus> hostVerifyStatusMap;
};

struct BundleVerifyStatusInfo : public Parcelable {
public:
    bool Marshalling(Parcel &parcel) const override;
    static BundleVerifyStatusInfo *Unmarshalling(Parcel &parcel);

private:
    bool WriteHostVerifyStatusMap(const std::unordered_map<std::string, InnerVerifyStatus> &hostVerifyStatusMap,
        Parcel &parcel) const;
    bool ReadHostVerifyStatusMap(std::unordered_map<std::string, InnerVerifyStatus> &hostVerifyStatusMap,
        Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);

public:
    std::unordered_map<std::string, VerifyResultInfo> bundleVerifyStatusInfoMap_;
};
}
}

#endif