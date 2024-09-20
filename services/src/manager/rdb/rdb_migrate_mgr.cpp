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
#include "rdb_migrate_mgr.h"
#include "app_domain_verify_hilog.h"
#include "app_domain_verify_hisysevent.h"

namespace OHOS::AppDomainVerify {
constexpr int INNER_VERSION_UNKNOWN = 0;
constexpr int INNER_VERSION_1_0 = 1;
constexpr int INNER_VERSION_1_0_COL_CNT = 2;

int RdbMigrateMgr::Upgrade(NativeRdb::RdbStore& rdbStore)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    int innerVersion = QueryInnerVersion(rdbStore);
    if (innerVersion == INNER_VERSION_1_0) {
        UpgradeFromV1_0(rdbStore);
    } else {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called, no need upgrade.");
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called, end");
    return true;
}
RdbMigrateMgr::RdbMigrateMgr(const AppDomainVerifyRdbConfig& appDomainVerifyRdbConfig)
{
    appDomainVerifyRdbConfig_ = appDomainVerifyRdbConfig;
}
int RdbMigrateMgr::QueryInnerVersion(NativeRdb::RdbStore& rdbStore)
{
    NativeRdb::AbsRdbPredicates absRdbPredicates(appDomainVerifyRdbConfig_.tableName);
    std::vector<std::string> columns = {};
    auto absSharedResultSet = rdbStore.Query(absRdbPredicates, columns);
    if (absSharedResultSet == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "QueryInnerVersion query absSharedResultSet failed");
        return INNER_VERSION_UNKNOWN;
    }
    int innerVersion = INNER_VERSION_UNKNOWN;
    int count = -1;
    absSharedResultSet->GetColumnCount(count);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdbStore GetColumnCount %{public}d", count);
    if (count == INNER_VERSION_1_0_COL_CNT) {
        innerVersion = INNER_VERSION_1_0;
    }
    absSharedResultSet->Close();
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "rdbStore innerVersion %{public}d", innerVersion);
    return innerVersion;
}

void RdbMigrateMgr::UpgradeFromV1_0(NativeRdb::RdbStore& rdbStore)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    const std::string migrateSqls[] = { R"(drop table IF EXISTS temp_table;)", R"(drop table IF EXISTS final_table;)",
        R"(CREATE TEMPORARY TABLE temp_table AS
            SELECT
              verified_domain.KEY,
              json_extract(verified_domain.VALUE, '$.appIdentifier') as appid,
              json_extract(verified_domain.VALUE, '$.hostVerifyStatusMap') as map
            FROM
              verified_domain;)",
        R"(CREATE TABLE final_table AS
            SELECT
              temp_table.KEY as BUNDLE_NAME,
              temp_table.appid as APP_IDENTIFIER,
              json_each.key AS DOMAIN,
              json_each.value AS VERIFY_STATUES
            FROM
              temp_table,
              json_each(temp_table.map);)",
        R"(drop table IF EXISTS verified_domain;)",
        R"(CREATE TABLE IF NOT EXISTS  verified_domain (ID INTEGER PRIMARY KEY AUTOINCREMENT, BUNDLE_NAME TEXT NOT NULL,
            APP_IDENTIFIER TEXT,
            DOMAIN TEXT NOT NULL, VERIFY_STATUES INTEGER);)",
        R"(INSERT INTO verified_domain (BUNDLE_NAME,APP_IDENTIFIER,DOMAIN,VERIFY_STATUES) select * from final_table;)",
        R"(drop table IF EXISTS final_table;)" };
    for (const auto& sql : migrateSqls) {
        auto ret = rdbStore.ExecuteSql(sql);
        if (ret != NativeRdb::E_OK) {
            APP_DOMAIN_VERIFY_HILOGE(
                APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "execute sql %{public}s failed.", sql.c_str());
            UNIVERSAL_ERROR_EVENT(UPDATE_DB_FAULT);
            auto dropTableSql = R"(drop table IF EXISTS verified_domain;)";
            (void)rdbStore.ExecuteSql(dropTableSql);
            APP_DOMAIN_VERIFY_HILOGE(
                APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "UpgradeFromV1_0 executeSql failed, ret: %{public}d", ret);
            break;
        }
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
}
}