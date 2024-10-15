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
#include <algorithm>
#include <string>
#include <utility>
#include "app_domain_verify_data_mgr.h"
#include "inner_verify_status.h"

namespace OHOS {
namespace AppDomainVerify {
AppDomainVerifyDataMgr::AppDomainVerifyDataMgr()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "new instance created.");
    verifyMap_ = std::make_shared<std::unordered_map<std::string, VerifyResultInfo>>();
    if (InitRdb()) {
        LoadAllFromRdb();
    }
}

AppDomainVerifyDataMgr::~AppDomainVerifyDataMgr()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "instance dead.");
}

bool AppDomainVerifyDataMgr::GetVerifyStatus(const std::string& bundleName, VerifyResultInfo& verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    std::string key;
    if (!GetParamKey(bundleName, key)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "key is empty");
        return false;
    }
    std::lock_guard<std::mutex> lock(verifyMapMutex_);
    auto it = verifyMap_->find(key);
    if (it != verifyMap_->end()) {
        verifyResultInfo = it->second;
        return true;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE,
        "get verify status fail, verify result can't find");
    return true;
}
bool AppDomainVerifyDataMgr::VerifyResultInfoToDB(
    const std::string bundleName, const VerifyResultInfo& verifyResultInfo)
{
    if (!rdbDataManager_->DeleteData(bundleName)) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "try delete bundleName failed.");
    }
    for (auto it : verifyResultInfo.hostVerifyStatusMap) {
        std::string domain = it.first;
        auto [status, verifyTime, cnt] = it.second;
        RdbDataItem item = { .bundleName = bundleName,
            .appIdentifier = verifyResultInfo.appIdentifier,
            .domain = domain,
            .status = status,
            .verifyTs = verifyTime,
            .count = cnt };
        if (!rdbDataManager_->InsertData(item)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "insert to db failed!");
            return false;
        }
    }
    return true;
}
bool AppDomainVerifyDataMgr::DBToVerifyResultInfo(
    const std::vector<RdbDataItem>& items, VerifyResultInfo& verifyResultInfo)
{
    if (items.empty()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "items empty!");
        return false;
    }
    verifyResultInfo.appIdentifier = items[0].appIdentifier;
    for (auto it : items) {
        verifyResultInfo.hostVerifyStatusMap.insert(std::make_pair(it.domain,
            std::make_tuple(InnerVerifyStatus(it.status), it.verifyTs, it.count)));
    }
    return true;
}

bool AppDomainVerifyDataMgr::SaveVerifyStatus(const std::string& bundleName, const VerifyResultInfo& verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    std::string key;
    if (!GetParamKey(bundleName, key)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "key is empty!");
        return false;
    }
    if (rdbDataManager_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdbDataManager is null");
        return false;
    }

    std::lock_guard<std::mutex> lock(verifyMapMutex_);
    UpdateVerifyMap(key, verifyResultInfo);
    auto completeBundleInfo = verifyMap_->find(key);
    if (completeBundleInfo == verifyMap_->end()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "InnerVerifyStatus save bundleInfo failed");
        return false;
    }
    if (!VerifyResultInfoToDB(key, completeBundleInfo->second)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "InnerVerifyStatus save to db failed");
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return true;
}

void AppDomainVerifyDataMgr::UpdateVerifyMap(const std::string& bundleName, const VerifyResultInfo& verifyResultInfo)
{
    auto bundleInfo = verifyMap_->find(bundleName);
    auto& hostVerifyStatusMap = verifyResultInfo.hostVerifyStatusMap;
    if (bundleInfo != verifyMap_->end()) {
        auto& hostVerifyStatusMapTarget = bundleInfo->second.hostVerifyStatusMap;
        std::for_each(hostVerifyStatusMap.begin(), hostVerifyStatusMap.end(),
            [&hostVerifyStatusMapTarget](auto iter) {
                hostVerifyStatusMapTarget.insert_or_assign(iter.first, iter.second);
            });
    } else {
        verifyMap_->insert_or_assign(bundleName, verifyResultInfo);
    }
}

bool AppDomainVerifyDataMgr::DeleteVerifyStatus(const std::string& bundleName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");

    std::string key;
    if (!GetParamKey(bundleName, key)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "key is empty!");
        return false;
    }
    if (rdbDataManager_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdbDataManager is null");
        return false;
    }
    std::lock_guard<std::mutex> lock(verifyMapMutex_);
    verifyMap_->erase(key);
    if (!rdbDataManager_->DeleteData(key)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "InnerVerifyStatus db delete failed");
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return true;
}

const std::unordered_map<std::string, VerifyResultInfo>& AppDomainVerifyDataMgr::GetAllVerifyStatus()
{
    std::lock_guard<std::mutex> lock(verifyMapMutex_);
    return *verifyMap_;
}

bool AppDomainVerifyDataMgr::GetParamKey(const std::string& bundleName, std::string& paramKey)
{
    if (bundleName.empty()) {
        return false;
    }
    paramKey = bundleName;
    return true;
}

bool AppDomainVerifyDataMgr::InitRdb()
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbDataManager_ = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    return rdbDataManager_->CreateTable();
}

bool AppDomainVerifyDataMgr::LoadAllFromRdb()
{
    std::unordered_map<std::string, std::vector<RdbDataItem>> dataMap;
    if (!rdbDataManager_->QueryAllData(dataMap)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "LoadAllFromRdb failed");
        return false;
    }
    for (auto it = dataMap.begin(); it != dataMap.end(); ++it) {
        VerifyResultInfo verifyResultInfo;
        DBToVerifyResultInfo(it->second, verifyResultInfo);
        verifyMap_->insert(std::make_pair(it->first, verifyResultInfo));
    }
    return true;
}
bool AppDomainVerifyDataMgr::QueryAssociatedDomains(const std::string& bundleName, std::vector<std::string>& domains)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    std::vector<RdbDataItem> items;
    if (!rdbDataManager_->QueryDomainByBundleName(bundleName, items)) {
        return false;
    }
    for (const auto& item : items) {
        domains.emplace_back(item.domain);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return true;
}
bool AppDomainVerifyDataMgr::QueryAssociatedBundleNames(
    const std::string& domain, std::vector<std::string>& bundleNames)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    std::vector<RdbDataItem> items;
    if (!rdbDataManager_->QueryBundleNameByDomain(domain, items)) {
        return false;
    }
    for (const auto& item : items) {
        bundleNames.emplace_back(item.bundleName);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return true;
}
}  // namespace AppDomainVerify
}  // namespace OHOS
