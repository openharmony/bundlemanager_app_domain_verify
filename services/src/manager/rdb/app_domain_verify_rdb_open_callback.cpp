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
#include "app_domain_verify_rdb_open_callback.h"
#include "app_domain_verify_hilog.h"
#include "rdb_errno.h"
#include "rdb_migrate_mgr.h"

namespace OHOS {
namespace AppDomainVerify {
AppDomainVerifyRdbOpenCallback::AppDomainVerifyRdbOpenCallback(const AppDomainVerifyRdbConfig& rdbConfig)
    : appDomainVerifyRdbConfig_(rdbConfig)
{
}
int32_t AppDomainVerifyRdbOpenCallback::OnCreate(NativeRdb::RdbStore& rdbStore)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "RdbStore OnCreate");
    return NativeRdb::E_OK;
}
int32_t AppDomainVerifyRdbOpenCallback::OnUpgrade(NativeRdb::RdbStore& rdbStore, int currentVersion, int targetVersion)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE,
        "OnUpgrade currentVersion: %{public}d, targetVersion: %{public}d", currentVersion, targetVersion);
    RdbMigrateMgr rdbMigrateMgr(appDomainVerifyRdbConfig_);
    int32_t ret = rdbMigrateMgr.Upgrade(rdbStore, currentVersion, targetVersion);
    if (ret != NativeRdb::E_OK) {
       APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "OnUpgrade fail., ret:%{public}d", ret);
       return ret;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "OnUpgrade End");
    return NativeRdb::E_OK;
}
int32_t AppDomainVerifyRdbOpenCallback::OnDowngrade(
    NativeRdb::RdbStore& rdbStore, int currentVersion, int targetVersion)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE,
        "OnDowngrade currentVersion: %{plubic}d, targetVersion: %{plubic}d", currentVersion, targetVersion);
    return NativeRdb::E_OK;
}
int32_t AppDomainVerifyRdbOpenCallback::OnOpen(NativeRdb::RdbStore& rdbStore)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "RdbStore OnOpen");
    return NativeRdb::E_OK;
}
int32_t AppDomainVerifyRdbOpenCallback::onCorruption(std::string databaseFile)
{
    return NativeRdb::E_OK;
}
}  // namespace AppDomainVerify
}  // namespace OHOS
