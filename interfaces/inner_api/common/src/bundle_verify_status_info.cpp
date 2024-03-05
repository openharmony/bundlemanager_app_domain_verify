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
#include "bundle_verify_status_info.h"
#include "app_domain_verify_hilog.h"

namespace OHOS {
namespace AppDomainVerify {
bool VerifyResultInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(appIdentifier)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteString for appIdentifier");
        return false;
    }
    if (!parcel.WriteUint32(hostVerifyStatusMap.size())) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteInt32 for hostVerifyStatusMap size");
        return false;
    }
    for (auto &it : hostVerifyStatusMap) {
        if (!parcel.WriteString(it.first)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteString for url");
            return false;
        }
        if (!parcel.WriteInt32(it.second)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteInt32 for verifyStatus");
            return false;
        }
    }
    return true;
}

VerifyResultInfo *VerifyResultInfo::Unmarshalling(Parcel &parcel)
{
    VerifyResultInfo *verifyResultInfo = new (std::nothrow) VerifyResultInfo();
    if ((verifyResultInfo != nullptr) && (!verifyResultInfo->ReadFromParcel(parcel))) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to read from parcel");
        delete verifyResultInfo;
        verifyResultInfo = nullptr;
    }
    return verifyResultInfo;
}

bool VerifyResultInfo::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(appIdentifier)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadString for appIdentifier");
        return false;
    }
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadUint32 for hostVerifyStatusMap size");
        return false;
    }
    hostVerifyStatusMap.clear();
    for (uint32_t index = 0; index < size; ++index) {
        std::string url;
        int verifyStatus = 0;
        if (!parcel.ReadString(url)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadString for url");
            return false;
        }
        if (!parcel.ReadInt32(verifyStatus)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadInt32 for verifyStatus");
            return false;
        }
        hostVerifyStatusMap.insert(std::make_pair(url, static_cast<InnerVerifyStatus>(verifyStatus)));
    }
    return true;
}

json VerifyResultInfo::VerifyResultInfoToJson(const VerifyResultInfo &verifyResultInfo)
{
    json verifyResultInfoJson;
    verifyResultInfoJson["appIdentifier"] = verifyResultInfo.appIdentifier;
    for (const auto &hostVerifyStatus : verifyResultInfo.hostVerifyStatusMap) {
        verifyResultInfoJson["hostVerifyStatusMap"][hostVerifyStatus.first] = hostVerifyStatus.second;
    }
    return verifyResultInfoJson;
}

VerifyResultInfo VerifyResultInfo::JsonToVerifyResultInfo(const json &verifyResultInfoJson)
{
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = verifyResultInfoJson["appIdentifier"].is_string() ?
        verifyResultInfoJson["appIdentifier"] :
        "";
    if (verifyResultInfoJson["hostVerifyStatusMap"].is_object()) {
        for (auto jsonIt = verifyResultInfoJson["hostVerifyStatusMap"].begin();
             jsonIt != verifyResultInfoJson["hostVerifyStatusMap"].end(); ++jsonIt) {
            verifyResultInfo.hostVerifyStatusMap.insert(std::make_pair(jsonIt.key(), jsonIt.value()));
        }
    }
    return verifyResultInfo;
}

bool BundleVerifyStatusInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint32(bundleVerifyStatusInfoMap_.size())) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteInt32 for whole map size");
        return false;
    }
    for (auto &it : bundleVerifyStatusInfoMap_) {
        if (!parcel.WriteString(it.first)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteString for bundlename");
            return false;
        }
        if (!parcel.WriteString(it.second.appIdentifier)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteString for appIdentifier");
            return false;
        }
        if (!WriteHostVerifyStatusMap(it.second.hostVerifyStatusMap, parcel)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteHostVerifyStatusMap");
            return false;
        }
    }
    return true;
}

bool BundleVerifyStatusInfo::WriteHostVerifyStatusMap(
    const std::unordered_map<std::string, InnerVerifyStatus> &hostVerifyStatusMap, Parcel &parcel) const
{
    if (!parcel.WriteUint32(hostVerifyStatusMap.size())) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteInt32 for hostVerifyStatusMap size");
        return false;
    }
    for (auto &it : hostVerifyStatusMap) {
        if (!parcel.WriteString(it.first)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteString for url");
            return false;
        }
        if (!parcel.WriteInt32(it.second)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteInt32 for verifyStatus");
            return false;
        }
    }
    return true;
}

bool BundleVerifyStatusInfo::ReadHostVerifyStatusMap(
    std::unordered_map<std::string, InnerVerifyStatus> &hostVerifyStatusMap, Parcel &parcel)
{
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadUint32 for hostVerifyStatusMap size");
        return false;
    }
    hostVerifyStatusMap.clear();
    for (uint32_t index = 0; index < size; ++index) {
        std::string url;
        int verifyStatus = 0;
        if (!parcel.ReadString(url)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadString for url");
            return false;
        }
        if (!parcel.ReadInt32(verifyStatus)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadInt32 for verifyStatus");
            return false;
        }
        hostVerifyStatusMap.insert(std::make_pair(url, static_cast<InnerVerifyStatus>(verifyStatus)));
    }
    return true;
}

BundleVerifyStatusInfo *BundleVerifyStatusInfo::Unmarshalling(Parcel &parcel)
{
    BundleVerifyStatusInfo *bundleVerifyStatusInfo = new (std::nothrow) BundleVerifyStatusInfo();
    if ((bundleVerifyStatusInfo != nullptr) && (!bundleVerifyStatusInfo->ReadFromParcel(parcel))) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to read from parcel");
        delete bundleVerifyStatusInfo;
        bundleVerifyStatusInfo = nullptr;
    }
    return bundleVerifyStatusInfo;
}

bool BundleVerifyStatusInfo::ReadFromParcel(Parcel &parcel)
{
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadInt32 for size");
        return false;
    }
    // todo 是否要设置包上限
    bundleVerifyStatusInfoMap_.clear();
    for (uint32_t index = 0; index < size; ++index) {
        std::string bundleName;
        if (!parcel.ReadString(bundleName)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadString for bundleName");
            return false;
        }
        VerifyResultInfo verifyResultInfo;
        if (!parcel.ReadString(verifyResultInfo.appIdentifier)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadString for appIdentifier");
            return false;
        }
        if (!ReadHostVerifyStatusMap(verifyResultInfo.hostVerifyStatusMap, parcel)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to ReadHostVerifyStatusMap");
            return false;
        }
        bundleVerifyStatusInfoMap_.insert(std::make_pair(bundleName, verifyResultInfo));
    }
    return true;
}
}
}
