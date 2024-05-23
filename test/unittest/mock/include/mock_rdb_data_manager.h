/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef APP_DOMAIN_VERIFY_MOCK_RDB_DATA_MANAGER_H
#define APP_DOMAIN_VERIFY_MOCK_RDB_DATA_MANAGER_H
#include "app_domain_verify_rdb_data_manager.h"
#include "gmock/gmock.h"
namespace OHOS::AppDomainVerify {
using DataMap = std::unordered_map<std::string, std::string>;
class IMocAppDomainVerifyRdbDataManagerImpl {
public:
    IMocAppDomainVerifyRdbDataManagerImpl()
    {
    }
    virtual ~IMocAppDomainVerifyRdbDataManagerImpl()
    {
    }
    virtual bool InsertData(const std::string& key, const std::string& value) = 0;
    virtual bool DeleteData(const std::string& key) = 0;
    virtual bool QueryAllData(DataMap& dataMap) = 0;
    virtual bool CreateTable() = 0;
};
class MocAppDomainVerifyRdbDataManagerImpl : public IMocAppDomainVerifyRdbDataManagerImpl {
public:
    MOCK_METHOD(bool, InsertData, (const std::string& key, const std::string& value), (override));
    MOCK_METHOD(bool, DeleteData, (const std::string& key), (override));
    MOCK_METHOD(bool, QueryAllData, (DataMap & dataMap), (override));
    MOCK_METHOD(bool, CreateTable, (), (override));
};
namespace MOC {
extern std::shared_ptr<MocAppDomainVerifyRdbDataManagerImpl> impl;
}
}

#endif  // APP_DOMAIN_VERIFY_MOCK_RDB_DATA_MANAGER_H
