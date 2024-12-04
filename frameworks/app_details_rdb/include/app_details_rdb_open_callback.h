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

#ifndef APP_DOMAIN_VERIFY_SERVICE_INCLUDE_APP_DETAILS_RDB_OPEN_CALLBACK_H
#define APP_DOMAIN_VERIFY_SERVICE_INCLUDE_APP_DETAILS_RDB_OPEN_CALLBACK_H

#include "app_details_rdb_data_define.h"
#include "rdb_errno.h"
#include "rdb_open_callback.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDetailsRdbOpenCallback : public NativeRdb::RdbOpenCallback {
public:
    AppDetailsRdbOpenCallback(const RdbConfigInfo &rdbConfig);
    int32_t OnCreate(NativeRdb::RdbStore &rdbStore) override;
    int32_t OnUpgrade(NativeRdb::RdbStore &rdbStore, int currentVersion, int targetVersion) override;
    int32_t OnDowngrade(NativeRdb::RdbStore &rdbStore, int currentVersion, int targetVersion) override;
    int32_t OnOpen(NativeRdb::RdbStore &rdbStore) override;
    int32_t onCorruption(std::string databaseFile) override;
private:
    RdbConfigInfo m_conf;
};
}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_SERVICE_INCLUDE_APP_DETAILS_RDB_OPEN_CALLBACK_H
