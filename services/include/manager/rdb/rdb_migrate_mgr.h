/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef APP_DOMAIN_VERIFY_RDB_MIGRATE_MGR_H
#define APP_DOMAIN_VERIFY_RDB_MIGRATE_MGR_H
#include "app_domain_verify_rdb_config.h"
#include "rdb_errno.h"
#include "rdb_open_callback.h"
#include "app_domain_verify_rdb_data_manager.h"
#include "bundle_verify_status_info.h"
#include "rdb_store.h"
#include <functional>
namespace OHOS::AppDomainVerify {
using TransFunc = std::function<bool(NativeRdb::RdbStore& rdb)>;
class RdbMigrateMgr {
public:
    RdbMigrateMgr(const AppDomainVerifyRdbConfig& appDomainVerifyRdbConfig);
    int Upgrade(NativeRdb::RdbStore& rdbStore, int currVersion, int targetVersion);

private:
    AppDomainVerifyRdbConfig appDomainVerifyRdbConfig_;
    int QueryInnerVersion(NativeRdb::RdbStore& store);
    void UpgradeFromV1ToV2(NativeRdb::RdbStore& rdbStore);
    int UpgradeFromV2ToV3(NativeRdb::RdbStore& rdbStore);
    int ExecSqlWithTrans(NativeRdb::RdbStore& store, const TransFunc& func);
};
}
#endif  // APP_DOMAIN_VERIFY_RDB_MIGRATE_MGR_H
