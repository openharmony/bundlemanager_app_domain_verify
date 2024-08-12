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

#include "app_domain_verify_rdb_data_manager.h"
#include <string>
#include "app_domain_verify_hilog.h"
#include "scope_guard.h"
#include "inner_verify_status.h"

namespace OHOS {
namespace AppDomainVerify {
const std::string DB_BUNDLE_NAME = "BUNDLE_NAME";
const std::string DB_APP_IDENTIFIER = "APP_IDENTIFIER";
const std::string DB_DOMAIN = "DOMAIN";
const std::string DB_STATUES = "VERIFY_STATUES";
const int32_t DB_BUNDLE_NAME_INDEX = 1;
const int32_t DB_APP_IDENTIFIER_INDEX = 2;
const int32_t DB_DOMAIN_INDEX = 3;
const int32_t DB_STATUES_INDEX = 4;
const int32_t CLOSE_TIME = 20;  // delay 20s stop rdbStore

AppDomainVerifyRdbDataManager::AppDomainVerifyRdbDataManager(const AppDomainVerifyRdbConfig& rdbConfig)
    : appDomainVerifyRdbConfig_(rdbConfig)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "new instance create.");
    DeleteIfCannotAccess();
}
void AppDomainVerifyRdbDataManager::DeleteIfCannotAccess()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "DeleteIfCannotAccess.");
    auto rdbFile = appDomainVerifyRdbConfig_.dbPath + appDomainVerifyRdbConfig_.dbName;
    // 文件存在但是没有读写权限，删除文件
    if (access(rdbFile.c_str(), F_OK) == 0 && access(rdbFile.c_str(), R_OK | W_OK) != 0) {
        auto ret = remove(rdbFile.c_str());
        APP_DOMAIN_VERIFY_HILOGW(
            APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "DeleteIfCannotAccess remove, ret:%{public}d.", ret);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "DeleteIfCannotAccess end.");
}
AppDomainVerifyRdbDataManager::~AppDomainVerifyRdbDataManager()
{
    rdbStore_ = nullptr;
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "instance dead.");
}

bool AppDomainVerifyRdbDataManager::InsertData(const RdbDataItem& rdbDataItem)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    auto rdbStore = GetRdbStore();
    if (!CheckRdbStoreExist(rdbStore)) {
        return false;
    }
    int64_t rowId = -1;
    NativeRdb::ValuesBucket valuesBucket;
    valuesBucket.PutString(DB_BUNDLE_NAME, rdbDataItem.bundleName);
    valuesBucket.PutString(DB_APP_IDENTIFIER, rdbDataItem.appIdentifier);
    valuesBucket.PutString(DB_DOMAIN, rdbDataItem.domain);
    valuesBucket.PutInt(DB_STATUES, rdbDataItem.status);
    auto ret = rdbStore->InsertWithConflictResolution(
        rowId, appDomainVerifyRdbConfig_.tableName, valuesBucket, NativeRdb::ConflictResolution::ON_CONFLICT_REPLACE);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return CheckRdbReturnIfOk(ret);
}

bool AppDomainVerifyRdbDataManager::QueryDomainByBundleName(
    const std::string& bundleName, std::vector<RdbDataItem>& items)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    NativeRdb::AbsRdbPredicates absRdbPredicates(appDomainVerifyRdbConfig_.tableName);
    absRdbPredicates.EqualTo(DB_BUNDLE_NAME, bundleName)->And()->EqualTo(DB_STATUES, InnerVerifyStatus::STATE_SUCCESS);
    std::vector<std::string> columns = {};
    return Query(absRdbPredicates, columns, items);
}
bool AppDomainVerifyRdbDataManager::QueryBundleNameByDomain(const std::string& domain, std::vector<RdbDataItem>& items)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    NativeRdb::AbsRdbPredicates absRdbPredicates(appDomainVerifyRdbConfig_.tableName);
    absRdbPredicates.EqualTo(DB_DOMAIN, domain)->And()->EqualTo(DB_STATUES, InnerVerifyStatus::STATE_SUCCESS);
    std::vector<std::string> columns = {};

    return Query(absRdbPredicates, columns, items);
}
bool AppDomainVerifyRdbDataManager::Query(const NativeRdb::AbsRdbPredicates& predicates,
    const std::vector<std::string>& columns, std::vector<RdbDataItem>& items)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    auto rdbStore = GetRdbStore();
    if (!CheckRdbStoreExist(rdbStore)) {
        return false;
    }
    auto absSharedResultSet = rdbStore->Query(predicates, columns);
    if (absSharedResultSet == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdbStore query absSharedResultSet failed");
        return false;
    }
    ScopeGuard stateGuard([&] { absSharedResultSet->Close(); });
    if (!absSharedResultSet->HasBlock()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "absSharedResultSet has no block");
        return false;
    }

    if (CheckRdbReturnIfOk(absSharedResultSet->GoToFirstRow())) {
        do {
            RdbDataItem item;
            if (!CheckRdbReturnIfOk(absSharedResultSet->GetString(DB_BUNDLE_NAME_INDEX, item.bundleName)) ||
                !CheckRdbReturnIfOk(absSharedResultSet->GetString(DB_APP_IDENTIFIER_INDEX, item.appIdentifier)) ||
                !CheckRdbReturnIfOk(absSharedResultSet->GetString(DB_DOMAIN_INDEX, item.domain)) ||
                !CheckRdbReturnIfOk(absSharedResultSet->GetInt(DB_STATUES_INDEX, item.status))) {
                return false;
            }
            items.emplace_back(item);
        } while (absSharedResultSet->GoToNextRow() == NativeRdb::E_OK);
    } else {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "GoToFirstRow fail, seems rdb table is empty");
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return true;
}
bool AppDomainVerifyRdbDataManager::DeleteData(const std::string& bundleName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    auto rdbStore = GetRdbStore();
    if (!CheckRdbStoreExist(rdbStore)) {
        return false;
    }
    int32_t rowId = -1;
    NativeRdb::AbsRdbPredicates absRdbPredicates(appDomainVerifyRdbConfig_.tableName);
    absRdbPredicates.EqualTo(DB_BUNDLE_NAME, bundleName);
    auto ret = rdbStore->Delete(rowId, absRdbPredicates);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return CheckRdbReturnIfOk(ret);
}

bool AppDomainVerifyRdbDataManager::QueryAllData(std::unordered_map<std::string, std::vector<RdbDataItem>>& dataMap)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    auto rdbStore = GetRdbStore();
    if (!CheckRdbStoreExist(rdbStore)) {
        return false;
    }
    NativeRdb::AbsRdbPredicates absRdbPredicates(appDomainVerifyRdbConfig_.tableName);
    std::vector<RdbDataItem> items;
    std::vector<std::string> columns = {};
    if (!Query(absRdbPredicates, columns, items)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "Query failed.");
        return false;
    }

    dataMap.clear();
    for (auto item : items) {
        if (dataMap.count(item.bundleName) != 0) {
            dataMap[item.bundleName].emplace_back(item);
        } else {
            std::vector<RdbDataItem> tmpItems;
            tmpItems.emplace_back(item);
            dataMap.insert(std::make_pair(item.bundleName, tmpItems));
        }
    }

    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyRdbDataManager::CreateTable()
{
    std::string createTableSql;
    if (appDomainVerifyRdbConfig_.createTableSql.empty()) {
        createTableSql = std::string("CREATE TABLE IF NOT EXISTS " + appDomainVerifyRdbConfig_.tableName +
            "(ID INTEGER PRIMARY KEY AUTOINCREMENT, BUNDLE_NAME TEXT NOT NULL, APP_IDENTIFIER TEXT, " +
            "DOMAIN TEXT NOT NULL, VERIFY_STATUES INTEGER);");
    } else {
        createTableSql = appDomainVerifyRdbConfig_.createTableSql;
    }
    auto rdbStore = GetRdbStore();
    if (CheckRdbStoreExist(rdbStore) && CheckRdbReturnIfOk(rdbStore->ExecuteSql(createTableSql))) {
        return true;
    }
    return false;
}

void AppDomainVerifyRdbDataManager::DelayCloseRdbStore()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    if (continuationHandler_ == nullptr) {
        continuationHandler_ = std::make_shared<ffrt::queue>("RdbContinuationMgr");
    }
    std::weak_ptr<AppDomainVerifyRdbDataManager> weakPtr = shared_from_this();
    auto func = [weakPtr]() {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "DelayCloseRdbStore thread begin");
        std::this_thread::sleep_for(std::chrono::seconds(CLOSE_TIME));
        auto sharedPtr = weakPtr.lock();
        if (sharedPtr == nullptr) {
            return;
        }
        std::lock_guard<std::mutex> lock(sharedPtr->rdbMutex_);
        sharedPtr->rdbStore_ = nullptr;
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "DelayCloseRdbStore thread end");
    };
    continuationHandler_->submit(func);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
}

std::shared_ptr<NativeRdb::RdbStore> AppDomainVerifyRdbDataManager::GetRdbStore()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    std::lock_guard<std::mutex> lock(rdbMutex_);
    if (rdbStore_ != nullptr) {
        return rdbStore_;
    }
    NativeRdb::RdbStoreConfig rdbStoreConfig(appDomainVerifyRdbConfig_.dbPath + appDomainVerifyRdbConfig_.dbName);
    rdbStoreConfig.SetSecurityLevel(NativeRdb::SecurityLevel::S1);
    int32_t errCode = NativeRdb::E_OK;
    AppDomainVerifyRdbOpenCallback appDomainVerifyRdbOpenCallback(appDomainVerifyRdbConfig_);
    rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(
        rdbStoreConfig, appDomainVerifyRdbConfig_.version, appDomainVerifyRdbOpenCallback, errCode);
    DelayCloseRdbStore();
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return rdbStore_;
}

bool AppDomainVerifyRdbDataManager::CheckRdbReturnIfOk(int errcode)
{
    if (errcode != NativeRdb::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdb failed, ret: %{public}d", errcode);
        return false;
    }
    return true;
}

bool AppDomainVerifyRdbDataManager::CheckRdbStoreExist(const std::shared_ptr<NativeRdb::RdbStore>& rdbStore)
{
    if (rdbStore == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "RdbStore is null");
        return false;
    }
    return true;
}
}  // namespace AppDomainVerify
}  // namespace OHOS