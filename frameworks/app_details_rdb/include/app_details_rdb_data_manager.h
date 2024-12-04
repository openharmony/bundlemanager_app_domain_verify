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

#ifndef APP_DOMAIN_VERIFY_SERVICE_INCLUDE_APP_DETAILS_RDB_DATA_MANAGER_H
#define APP_DOMAIN_VERIFY_SERVICE_INCLUDE_APP_DETAILS_RDB_DATA_MANAGER_H

#include <mutex>
#include "cpp/mutex.h"
#include "event_handler.h"
#include "rdb_store.h"
#include "values_bucket.h"
#include "app_details_rdb_data_define.h"
#include "ffrt.h"

namespace OHOS {
namespace AppDomainVerify {
using RdbForEachRetCb = std::function<bool(std::shared_ptr<AbsSharedResultSet> retSet)>;
class AppDetailsRdbDataMgr : public std::enable_shared_from_this<AppDetailsRdbDataMgr> {
public:
    AppDetailsRdbDataMgr(bool createFile);
    AppDetailsRdbDataMgr(const RdbConfigInfo& info);
    virtual ~AppDetailsRdbDataMgr();
    bool ExecWithTrans(TransCallback cb);
    bool InsertDataBatch(const std::string& tableName, std::vector<AppDetailsRdbItem>& itemVec);
    bool QueryDataByDomain(const std::string& tableName,
        const std::string& domain, std::vector<AppDetailsRdbItem>& itemVec);
    bool UpdateMetaData(std::vector<MetaItem>& item);
    bool QueryMetaData(const std::string& tableName, MetaItem& info);
    bool CreateMetaData();
    bool CreateTable(const std::string& tableName);
    bool DeleteTable(const std::string& tableName);
    bool RenameTable(const std::string& oldName, const std::string& newName);
    std::string GetDbVersion();
private:
    void PostDelayCloseTask(int32_t delayTime);
    bool Query(const NativeRdb::AbsRdbPredicates& predicates,
                const std::vector<std::string>& columns, RdbForEachRetCb cb);
    bool CheckRdbReturnIfOk(int errcode);
    std::shared_ptr<NativeRdb::RdbStore> GetRdbStore();
private:
    ffrt::mutex mtx_;
    std::shared_ptr<NativeRdb::RdbStore> rdbStore_;
    RdbConfigInfo conf_;
    std::shared_ptr<AppExecFwk::EventHandler> closeHandler_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
};

}  // namespace AppDomainVerify
}  // namespace OHOS
#endif
