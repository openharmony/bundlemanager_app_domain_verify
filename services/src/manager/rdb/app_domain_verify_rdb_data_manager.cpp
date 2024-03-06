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

namespace OHOS {
namespace AppDomainVerify {
const std::string DB_KEY = "KEY";
const std::string DB_VALUE = "VALUE";
const int32_t DB_KEY_INDEX = 0;
const int32_t DB_VALUE_INDEX = 1;
const int32_t CLOSE_TIME = 20;  // delay 20s stop rdbStore

#define CHECK_RDB_RESULT_RETURN_IF_OK(errmsg, errcode)                                   \
    do {                                                                                 \
        if ((errcode) != NativeRdb::E_OK) {                                              \
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, errmsg, errcode); \
            return false;                                                                \
        }                                                                                \
    } while (0)

#define CHECK_RDBSTORE_EXIST(rdbStore)                                                      \
    do {                                                                                    \
        if ((rdbStore) == nullptr) {                                                        \
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "RdbStore is null"); \
            return false;                                                                   \
        }                                                                                   \
    } while (0)

AppDomainVerifyRdbDataManager::AppDomainVerifyRdbDataManager(const AppDomainVerifyRdbConfig &rdbConfig)
    : appDomainVerifyRdbConfig_(rdbConfig)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "new instance create.");
}

AppDomainVerifyRdbDataManager::~AppDomainVerifyRdbDataManager()
{
    rdbStore_ = nullptr;
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "instance dead.");
}

bool AppDomainVerifyRdbDataManager::InsertData(const std::string &key, const std::string &value)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    auto rdbStore = GetRdbStore();
    CHECK_RDBSTORE_EXIST(rdbStore);
    int64_t rowId = -1;
    NativeRdb::ValuesBucket valuesBucket;
    valuesBucket.PutString(DB_KEY, key);
    valuesBucket.PutString(DB_VALUE, value);
    auto ret = rdbStore->InsertWithConflictResolution(rowId, appDomainVerifyRdbConfig_.tableName, valuesBucket,
        NativeRdb::ConflictResolution::ON_CONFLICT_REPLACE);

    CHECK_RDB_RESULT_RETURN_IF_OK("InsertData failed, ret: %{public}d", ret);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyRdbDataManager::DeleteData(const std::string &key)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    auto rdbStore = GetRdbStore();
    CHECK_RDBSTORE_EXIST(rdbStore);

    int32_t rowId = -1;
    NativeRdb::AbsRdbPredicates absRdbPredicates(appDomainVerifyRdbConfig_.tableName);
    absRdbPredicates.EqualTo(DB_KEY, key);
    auto ret = rdbStore->Delete(rowId, absRdbPredicates);

    CHECK_RDB_RESULT_RETURN_IF_OK("DeleteData failed, ret: %{public}d", ret);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyRdbDataManager::QueryAllData(std::unordered_map<std::string, std::string> &dataMap)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    auto rdbStore = GetRdbStore();
    CHECK_RDBSTORE_EXIST(rdbStore);

    NativeRdb::AbsRdbPredicates absRdbPredicates(appDomainVerifyRdbConfig_.tableName);
    auto absSharedResultSet = rdbStore->Query(absRdbPredicates, std::vector<std::string>());
    if (absSharedResultSet == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdbStore query absSharedResultSet failed");
        return false;
    }
    ScopeGuard stateGuard([&] { absSharedResultSet->Close(); });
    if (!absSharedResultSet->HasBlock()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "absSharedResultSet has no block");
        return false;
    }

    auto ret = absSharedResultSet->GoToFirstRow();
    CHECK_RDB_RESULT_RETURN_IF_OK("GoToFirstRow failed, ret: %{public}d", ret);
    do {
        std::string key;
        std::string value;
        auto keyRet = absSharedResultSet->GetString(DB_KEY_INDEX, key);
        auto valueRet = absSharedResultSet->GetString(DB_VALUE_INDEX, value);
        CHECK_RDB_RESULT_RETURN_IF_OK("Get rdb item key failed, ret: %{public}d", keyRet);
        CHECK_RDB_RESULT_RETURN_IF_OK("Get rdb item value failed, ret: %{public}d", valueRet);
        dataMap.insert(std::make_pair(key, value));
    } while (absSharedResultSet->GoToNextRow() == NativeRdb::E_OK);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyRdbDataManager::CreateTable()
{
    std::string createTableSql;
    if (appDomainVerifyRdbConfig_.createTableSql.empty()) {
        createTableSql = std::string("CREATE TABLE IF NOT EXISTS " + appDomainVerifyRdbConfig_.tableName +
            "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");
    } else {
        createTableSql = appDomainVerifyRdbConfig_.createTableSql;
    }
    auto rdbStore = GetRdbStore();
    CHECK_RDBSTORE_EXIST(rdbStore);
    int ret = rdbStore->ExecuteSql(createTableSql);
    CHECK_RDB_RESULT_RETURN_IF_OK("CreateTable failed, ret: %{public}d", ret);

    for (const auto &sql : appDomainVerifyRdbConfig_.insertColumnSql) {
        int32_t insertRet = rdbStore->ExecuteSql(sql);
        if (insertRet != NativeRdb::E_OK) {
            APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE,
                "ExecuteSql insertColumnSql failed, insertRet: %{public}d", insertRet);
        }
    }
    return true;
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
    rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(rdbStoreConfig, appDomainVerifyRdbConfig_.version,
        appDomainVerifyRdbOpenCallback, errCode);
    DelayCloseRdbStore();
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return rdbStore_;
}
}  // namespace AppDomainVerify
}  // namespace OHOS