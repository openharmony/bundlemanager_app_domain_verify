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
#include <string>
#include <utility>
#include "app_domain_verify_data_mgr.h"

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

bool AppDomainVerifyDataMgr::GetVerifyStatus(const std::string &bundleName, VerifyResultInfo &verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::string key;
    if (!GetParamKey(bundleName, key)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "key is empty");
        return false;
    }
    std::lock_guard<std::mutex> lock(verifyMapMutex_);
    auto it = verifyMap_->find(key);
    if (it != verifyMap_->end()) {
        verifyResultInfo = it->second;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyDataMgr::SaveVerifyStatus(const std::string &bundleName, const VerifyResultInfo &verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
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
    verifyMap_->insert_or_assign(key, verifyResultInfo);
    if (!rdbDataManager_->InsertData(key, VerifyResultInfo::VerifyResultInfoToJson(verifyResultInfo).dump())) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "InnerVerifyStatus save to db failed");
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyDataMgr::DeleteVerifyStatus(const std::string &bundleName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);

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
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return true;
}

const std::unordered_map<std::string, VerifyResultInfo> &AppDomainVerifyDataMgr::GetAllVerifyStatus()
{
    std::lock_guard<std::mutex> lock(verifyMapMutex_);
    return *verifyMap_;
}

bool AppDomainVerifyDataMgr::GetParamKey(const std::string &bundleName, std::string &paramKey)
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
    std::unordered_map<std::string, std::string> dataMap;
    if (!rdbDataManager_->QueryAllData(dataMap)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "LoadAllFromRdb failed");
        return false;
    }
    for (auto it = dataMap.begin(); it != dataMap.end(); ++it) {
        json verifyResultInfoJson;
        try {
            verifyResultInfoJson = json::parse(it->second);
        } catch (json::parse_error &e) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "hostVerifyStatusJson can not be parsed.");
            continue;
        }
        verifyMap_->insert(std::make_pair(it->first, VerifyResultInfo::JsonToVerifyResultInfo(verifyResultInfoJson)));
    }
    return true;
}
}  // namespace AppDomainVerify
}  // namespace OHOS
