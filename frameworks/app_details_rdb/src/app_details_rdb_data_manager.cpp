/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "app_details_rdb_data_manager.h"
#include "app_details_base_item.h"
#include "app_details_meta_item.h"
#include "app_details_rdb_data_define.h"
#include "app_details_rdb_item.h"
#include "app_details_rdb_open_callback.h"
#include "app_domain_verify_hilog.h"
#include "cpp/mutex.h"
#include "rdb_errno.h"
#include "rdb_store.h"
#include "values_bucket.h"
#include "rdb_helper.h"
#include <functional>
#include <string>
#include <variant>
#include <vector>
#include "app_details_rdb_const_define.h"
#include "utils/critial_utils.h"

namespace OHOS {
namespace AppDomainVerify {
AppDetailsRdbDataMgr::AppDetailsRdbDataMgr(bool isCreated)
{
    conf_.rdbName = APP_DETAILS_RDB_NAME;
    conf_.rdbPath = APP_DETAILS_RDB_PATH;
    conf_.version = APP_DETAILS_RDB_VERSION;
    conf_.fileCreated = isCreated;
};

AppDetailsRdbDataMgr::AppDetailsRdbDataMgr(const RdbConfigInfo& info)
{
    conf_ = info;
};

AppDetailsRdbDataMgr::~AppDetailsRdbDataMgr()
{
    if (closeHandler_ != nullptr) {
        closeHandler_->RemoveTask(CLOSE_TASK_ID);
    }
};

bool AppDetailsRdbDataMgr::ExecWithTrans(TransCallback cb)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call");
    auto rdbStore = GetRdbStore();
    CriticalLock criticalLock;
    auto ret = rdbStore->BeginTransaction();
    if (ret != NativeRdb::E_OK) {
        return false;
    }
    if (cb()) {
        ret = rdbStore->Commit();
        if (ret != NativeRdb::E_OK) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "commit err:%{public}d", ret);
            return false;
        }
        return true;
    }
    ret = rdbStore->RollBack();
    if (ret != NativeRdb::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "roll err:%{public}d.", ret);
    }
    return false;
}

bool AppDetailsRdbDataMgr::QueryDataByDomain(
    const std::string& tableName, const std::string &domain, std::vector<AppDetailsRdbItem> &itemVec)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call.");
    NativeRdb::AbsRdbPredicates absRdbPred(tableName);
    absRdbPred.EqualTo(DETAILS_DOMAIN, domain);
    std::vector<std::string> columns = {};
    RdbForEachRetCb eachCb = [&itemVec](std::shared_ptr<AbsSharedResultSet> retSet)->bool {
        AppDetailsRdbItem item;
        if (!item.GetRdbItem(retSet)) {
            return false;
        }
        itemVec.emplace_back(item);
        return true;
    };
    return Query(absRdbPred, columns, eachCb);
}

void AppDetailsRdbDataMgr::PostDelayCloseTask(int32_t delayTime)
{
    if (runner_ == nullptr) {
        runner_ = AppExecFwk::EventRunner::Create("closeRdb", AppExecFwk::ThreadMode::FFRT);
    }
    if (closeHandler_ == nullptr) {
        closeHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    }
    if (closeHandler_ == nullptr) {
        return;
    }
    std::weak_ptr<AppDetailsRdbDataMgr> weakPtr = shared_from_this();
    auto task = [weakPtr]() {
        auto appDetailsRdbMgr = weakPtr.lock();
        if (appDetailsRdbMgr != nullptr) {
            std::lock_guard<ffrt::mutex> lock(appDetailsRdbMgr->mtx_);
            appDetailsRdbMgr->rdbStore_ = nullptr;
            APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdb close");
        }
        return;
    };
    closeHandler_->RemoveTask(CLOSE_TASK_ID);
    closeHandler_->PostTask(task, CLOSE_TASK_ID, delayTime);
    return;
}

bool AppDetailsRdbDataMgr::InsertDataBatch(const std::string& tableName, std::vector<AppDetailsRdbItem>& infoVec)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    auto rdbStore = GetRdbStore();
    if (!rdbStore) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get rdbStore fail.");
        return false;
    }
    std::vector<NativeRdb::ValuesBucket> rows;
    for (auto& info : infoVec) {
        NativeRdb::ValuesBucket valuesBucket;
        info.AddRdbItemToBucket(valuesBucket);
        rows.emplace_back(valuesBucket);
    }
    int64_t num = 0;
    CriticalLock criticalLock;
    return CheckRdbReturnIfOk(rdbStore_->BatchInsert(num, tableName, rows));
};

bool AppDetailsRdbDataMgr::CreateMetaData()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    auto rdbStore = GetRdbStore();
    if (!rdbStore) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get rdbStore fail.");
        return false;
    }
    std::string sql = "CREATE TABLE IF NOT EXISTS " + META_DATA +
                        "("
                            "TABLE_NAME TEXT NOT NULL PRIMARY KEY,"
                            "TABLE_VERSION TEXT,"
                            "TABLE_EXT_INFO TEXT,"
                            "UPDATE_TIME TEXT"
                        ");";
    CriticalLock lock;
    auto ret = rdbStore->ExecuteSql(sql);
    if (ret != NativeRdb::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "exec sql fail. ret:%{public}d", ret);
        return false;
    }
    return true;
};

bool AppDetailsRdbDataMgr::UpdateMetaData(std::vector<MetaItem> &itemVec)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    auto rdbStore = GetRdbStore();
    if (!rdbStore) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get rdbStore fail.");
        return false;
    }
    CriticalLock lock;
    for (auto& item : itemVec) {
        NativeRdb::ValuesBucket valuesBucket;
        item.AddRdbItemToBucket(valuesBucket);
        int64_t rowId = 0;
        auto ret = rdbStore->InsertWithConflictResolution(rowId, META_DATA, valuesBucket,
            NativeRdb::ConflictResolution::ON_CONFLICT_REPLACE);
        if (ret != NativeRdb::E_OK) {
            return CheckRdbReturnIfOk(ret);
        }
    }
    return true;
};

bool AppDetailsRdbDataMgr::QueryMetaData(const std::string &tableName, MetaItem &info)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call.");
    NativeRdb::AbsRdbPredicates absRdbPred(META_DATA);
    absRdbPred.EqualTo(META_TABLE_NAME, tableName);
    std::vector<std::string> columns = {};
    auto rdbStore = GetRdbStore();
    if (!rdbStore) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get rdbStore fail.");
        return false;
    }
    std::vector<MetaItem> itemVec;
    RdbForEachRetCb eachCb = [&itemVec](std::shared_ptr<AbsSharedResultSet> retSet)->bool {
        MetaItem item;
        if (!item.GetRdbItem(retSet)) {
            return false;
        }
        itemVec.emplace_back(item);
        return true;
    };
    if (Query(absRdbPred, columns, eachCb)) {
        info = itemVec.front();
        return true;
    };
    return false;
};

bool AppDetailsRdbDataMgr::CreateTable(const std::string &tableName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName
                    + " ("
                            "ID TEXT NOT NULL PRIMARY KEY,"
                            "SCHEME TEXT,"
                            "DOMAIN TEXT,"
                            "PATH_TYPE TEXT,"
                            "PATH TEXT,"
                            "BUNDLE_NAME TEXT"
                    + " );";
    auto rdbStore = GetRdbStore();
    if (!rdbStore) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get rdbStore fail.");
        return false;
    }
    CriticalLock lock;
    auto ret = rdbStore->ExecuteSql(sql);
    if (ret != NativeRdb::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "exec sql fail. ret:%{public}d", ret);
        return false;
    }
    return true;
};

bool AppDetailsRdbDataMgr::CreateRegularIndex(const std::string& tableName, const std::string& colName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    std::string sql = "CREATE INDEX IF NOT EXISTS " + colName + "_INDEX " + "ON " + tableName + " (" + colName + ");";
    auto rdbStore = GetRdbStore();
    if (!rdbStore) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get rdbStore fail.");
        return false;
    }
    CriticalLock lock;
    auto ret = rdbStore->ExecuteSql(sql);
    if (ret != NativeRdb::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "exec sql fail. ret:%{public}d", ret);
        return false;
    }
    return true;
};

bool AppDetailsRdbDataMgr::DeleteTable(const std::string &tableName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "Called");
    std::string sql = "DROP TABLE IF EXISTS " + tableName + ";";
    auto rdbStore = GetRdbStore();
    if (!rdbStore) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "Get rdbStore fail.");
        return false;
    }
    CriticalLock lock;
    auto ret = rdbStore->ExecuteSql(sql);
    if (ret != NativeRdb::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "Exec sql fail. ret:%{public}d", ret);
        return false;
    }
    return true;
};

std::string AppDetailsRdbDataMgr::GetDbVersion()
{
    auto rdbStore = GetRdbStore();
    if (!rdbStore) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get rdbStore fail.");
        return "";
    }
    int version = -1;
    rdbStore->GetVersion(version);
    if (version != -1) {
        return std::to_string(version);
    }
    return "";
};

bool AppDetailsRdbDataMgr::RenameTable(const std::string &oldName, const std::string &newName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    std::string sql = "ALTER TABLE " + oldName + " RENAME TO " + newName + ";";
    auto rdbStore = GetRdbStore();
    if (!rdbStore) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get rdbStore fail.");
        return false;
    }
    CriticalLock lock;
    auto ret = rdbStore->ExecuteSql(sql);
    if (ret != NativeRdb::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "exec sql fail. ret:%{public}d", ret);
        return false;
    }
    return true;
};

std::shared_ptr<NativeRdb::RdbStore> AppDetailsRdbDataMgr::GetRdbStore()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    std::lock_guard<ffrt::mutex> lock(mtx_);
    if (rdbStore_ != nullptr) {
        return rdbStore_;
    }
    NativeRdb::RdbStoreConfig rdbStoreConfig(conf_.rdbPath + conf_.rdbName);
    rdbStoreConfig.SetCreateNecessary(conf_.fileCreated);
    rdbStoreConfig.SetSecurityLevel(NativeRdb::SecurityLevel::S1);
    int32_t errCode = NativeRdb::E_OK;
    AppDetailsRdbOpenCallback appDomainVerifyRdbOpenCallback(conf_);
    rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(
        rdbStoreConfig, conf_.version, appDomainVerifyRdbOpenCallback, errCode);
    if (errCode == NativeRdb::E_OK) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get rdbStore success.");
        PostDelayCloseTask(CLOSE_DELAY_TIME_S);
    } else {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get rdbStore fail, err:%{public}d", errCode);
    }
    return rdbStore_;
};

bool AppDetailsRdbDataMgr::CheckRdbReturnIfOk(int errcode)
{
    if (errcode != NativeRdb::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdb failed, ret: %{public}d", errcode);
        return false;
    }
    return true;
}

bool AppDetailsRdbDataMgr::Query(const NativeRdb::AbsRdbPredicates& predicates,
    const std::vector<std::string>& columns, RdbForEachRetCb cb)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    auto rdbStore = GetRdbStore();
    if (!rdbStore) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdb store is null");
        return false;
    }
    auto absSharedResultSet = rdbStore->Query(predicates, columns);
    if (absSharedResultSet == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdbStore query absSharedResultSet failed");
        return false;
    }
    auto guard = std::unique_ptr<void, std::function<void(void*)>>(nullptr,
        [&](void*) {
            absSharedResultSet->Close();
        });
    if (!absSharedResultSet->HasBlock()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "absSharedResultSet has no block");
        return false;
    }
    if (CheckRdbReturnIfOk(absSharedResultSet->GoToFirstRow())) {
        do {
            if (!cb(absSharedResultSet)) {
                return false;
            }
        } while (absSharedResultSet->GoToNextRow() == NativeRdb::E_OK);
    } else {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "GoToFirstRow fail, seems rdb table empty");
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return true;
};

}
}
