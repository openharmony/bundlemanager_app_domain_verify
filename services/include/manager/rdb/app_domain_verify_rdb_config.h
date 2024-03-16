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

#ifndef APP_DOMAIN_VERIFY_SERVICE_INCLUDE_RDB_CONFIG_H
#define APP_DOMAIN_VERIFY_SERVICE_INCLUDE_RDB_CONFIG_H

#include <string>
#include <vector>

#include "manager_constants.h"
#include "rdb_store_config.h"

namespace OHOS {
namespace AppDomainVerify {
struct AppDomainVerifyRdbConfig {
    std::string dbPath{ Constants::SERVICE_PATH };
    std::string dbName;
    std::string tableName;
    std::string createTableSql;
    int32_t version{ Constants::RDB_VERSION };
};
}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_SERVICE_INCLUDE_RDB_CONFIG_H
