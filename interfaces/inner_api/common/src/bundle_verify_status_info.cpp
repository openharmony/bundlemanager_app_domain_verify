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
#include "app_domain_verify_parcel_util.h"
#include "inner_verify_status.h"
#include <tuple>
namespace OHOS {
namespace AppDomainVerify {
bool VerifyResultInfo::Marshalling(Parcel& parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, appIdentifier);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, hostVerifyStatusMap.size());
    for (auto& it : hostVerifyStatusMap) {
        std::string domain = it.first;
        auto [status, verifyTime, cnt] = it.second;
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, domain);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, status);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, verifyTime);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, cnt);
    }
    return true;
}

VerifyResultInfo* VerifyResultInfo::Unmarshalling(Parcel& parcel)
{
    VerifyResultInfo* verifyResultInfo = new (std::nothrow) VerifyResultInfo();
    if ((verifyResultInfo != nullptr) && (!verifyResultInfo->ReadFromParcel(parcel))) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to read from parcel");
        delete verifyResultInfo;
        verifyResultInfo = nullptr;
    }
    return verifyResultInfo;
}

bool VerifyResultInfo::ReadFromParcel(Parcel& parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, appIdentifier);
    uint32_t size = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, size);
    if (IsInvalidParcelArraySize(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable size invalid.");
        return false;
    }
    hostVerifyStatusMap.clear();
    for (uint32_t index = 0; index < size; ++index) {
        std::string url;
        int verifyStatus = 0;
        std::string verifyTs;
        int count = 0;
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, url);
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, verifyStatus);
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, verifyTs);
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, count);
        hostVerifyStatusMap.insert(std::make_pair(
            url, std::make_tuple(static_cast<InnerVerifyStatus>(verifyStatus), verifyTs, count)));
    }
    return true;
}

std::string VerifyResultInfo::Dump() const
{
    std::string dumpStr = "appIdentifier:" + appIdentifier + "\n";
    for (const auto& hostVerifyStatus : hostVerifyStatusMap) {
        InnerVerifyStatus status;
        std::string verifyTime;
        int count = 0;
        std::tie(status, verifyTime, count) = hostVerifyStatus.second;
        dumpStr = dumpStr + "    " + "domain:" + hostVerifyStatus.first +
            " status:" + std::to_string(status) + " verifyTime:" + verifyTime + ";\n";
    }
    return dumpStr;
}

bool BundleVerifyStatusInfo::Marshalling(Parcel& parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, bundleVerifyStatusInfoMap_.size());
    for (auto& it : bundleVerifyStatusInfoMap_) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, it.first);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, it.second.appIdentifier);
        if (!WriteHostVerifyStatusMap(it.second.hostVerifyStatusMap, parcel)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to WriteHostVerifyStatusMap");
            return false;
        }
    }
    return true;
}

bool BundleVerifyStatusInfo::WriteHostVerifyStatusMap(
    const HostVerifyStatusMap& hostVerifyStatusMap, Parcel& parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, hostVerifyStatusMap.size());
    for (auto& it : hostVerifyStatusMap) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, it.first);
        auto [status, verifyTime, cnt] = it.second;
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, status);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, verifyTime);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, cnt);
    }
    return true;
}

bool BundleVerifyStatusInfo::ReadHostVerifyStatusMap(
    HostVerifyStatusMap& hostVerifyStatusMap, Parcel& parcel)
{
    uint32_t size = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, size);
    if (IsInvalidParcelArraySize(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable size invalid.");
        return false;
    }
    hostVerifyStatusMap.clear();
    for (uint32_t index = 0; index < size; ++index) {
        std::string url;
        int verifyStatus = 0;
        std::string verifyTs;
        int verifyCnt = 0;
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, url);
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, verifyStatus);
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, verifyTs);
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, verifyCnt);
        hostVerifyStatusMap.insert(
            std::make_pair(url, std::make_tuple(static_cast<InnerVerifyStatus>(verifyStatus), verifyTs, verifyCnt)));
    }
    return true;
}

BundleVerifyStatusInfo* BundleVerifyStatusInfo::Unmarshalling(Parcel& parcel)
{
    BundleVerifyStatusInfo* bundleVerifyStatusInfo = new (std::nothrow) BundleVerifyStatusInfo();
    if ((bundleVerifyStatusInfo != nullptr) && (!bundleVerifyStatusInfo->ReadFromParcel(parcel))) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "failed to read from parcel");
        delete bundleVerifyStatusInfo;
        bundleVerifyStatusInfo = nullptr;
    }
    return bundleVerifyStatusInfo;
}

bool BundleVerifyStatusInfo::ReadFromParcel(Parcel& parcel)
{
    uint32_t size = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, size);
    if (IsInvalidParcelArraySize(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable size invalid.");
        return false;
    }
    bundleVerifyStatusInfoMap_.clear();
    for (uint32_t index = 0; index < size; ++index) {
        std::string bundleName;
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName);
        VerifyResultInfo verifyResultInfo;
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, verifyResultInfo.appIdentifier);
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
