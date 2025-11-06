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
struct RdbDataItem {
    std::string bundleName;
    std::string appIdentifier;
    std::string domain;
    int status = 0;
    std::string verifyTs;
    int count = 0;
    int priority = 0;
};

class AppDomainVerifyRdbDataManager : public std::enable_shared_from_this<AppDomainVerifyRdbDataManager> {
public:
    AppDomainVerifyRdbDataManager(const AppDomainVerifyRdbConfig& rdbConfig);
    virtual ~AppDomainVerifyRdbDataManager();
    bool InsertData(const RdbDataItem& rdbDataItem);
    bool DeleteData(const std::string& bundleName);
    bool QueryAllData(std::unordered_map<std::string, std::vector<RdbDataItem>>& dataMap);
    bool QueryBundleNameByDomain(const std::string& domain, std::vector<RdbDataItem>& items);
    bool QueryDomainByBundleName(const std::string& bundleName, std::vector<RdbDataItem>& items);
    bool CreateTable();

private:
    void DeleteIfCannotAccess();
    std::shared_ptr<NativeRdb::RdbStore> GetRdbStore();
    void DelayCloseRdbStore();
    bool CheckRdbReturnIfOk(int errcode);
    bool CheckRdbStoreExist(const std::shared_ptr<NativeRdb::RdbStore>& rdbStore);
    bool Query(const NativeRdb::AbsRdbPredicates& predicates, const std::vector<std::string>& columns,
        std::vector<RdbDataItem>& items);

private:
    std::mutex rdbMutex_;
    std::shared_ptr<NativeRdb::RdbStore> rdbStore_;
    AppDomainVerifyRdbConfig appDomainVerifyRdbConfig_;
    std::shared_ptr<ffrt::queue> continuationHandler_;
};
}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_SERVICE_INCLUDE_RDB_DATA_MANAGER_H
