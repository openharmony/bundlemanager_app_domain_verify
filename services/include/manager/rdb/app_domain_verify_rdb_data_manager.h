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

#ifndef APP_DOMAIN_VERIFY_SERVICE_INCLUDE_RDB_DATA_MANAGER_H
#define APP_DOMAIN_VERIFY_SERVICE_INCLUDE_RDB_DATA_MANAGER_H

#include <mutex>

#include "app_domain_verify_rdb_config.h"
#include "app_domain_verify_rdb_open_callback.h"
#include "rdb_helper.h"
#include "values_bucket.h"
#include "ffrt.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyRdbDataManager : public std::enable_shared_from_this<AppDomainVerifyRdbDataManager> {
public:
    AppDomainVerifyRdbDataManager(const AppDomainVerifyRdbConfig &rdbConfig);
    virtual ~AppDomainVerifyRdbDataManager();
    bool InsertData(const std::string &key, const std::string &value);
    bool DeleteData(const std::string &key);
    bool QueryAllData(std::unordered_map<std::string, std::string> &dataMap);
    bool CreateTable();

private:
    std::shared_ptr<NativeRdb::RdbStore> GetRdbStore();
    void DelayCloseRdbStore();
    bool CheckRdbReturnIfOk(int errcode);
    bool CheckRdbStoreExist(const std::shared_ptr<NativeRdb::RdbStore> &rdbStore);

private:
    std::mutex rdbMutex_;
    std::shared_ptr<NativeRdb::RdbStore> rdbStore_;
    AppDomainVerifyRdbConfig appDomainVerifyRdbConfig_;
    std::shared_ptr<ffrt::queue> continuationHandler_;
};
}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_SERVICE_INCLUDE_RDB_DATA_MANAGER_H
