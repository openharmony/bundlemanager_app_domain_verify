/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "mock_rdb_data_manager.h"
namespace OHOS::AppDomainVerify {
namespace MOC {
std::shared_ptr<MocAppDomainVerifyRdbDataManagerImpl> impl = nullptr;
}
bool AppDomainVerifyRdbDataManager::InsertData(const RdbDataItem& rdbDataItem)
{
    if (!MOC::impl) {
        return false;
    }
    return MOC::impl->InsertData(rdbDataItem);
}
bool AppDomainVerifyRdbDataManager::DeleteData(const std::string& key)
{
    if (!MOC::impl) {
        return false;
    }
    return MOC::impl->DeleteData(key);
}
bool AppDomainVerifyRdbDataManager::QueryAllData(std::unordered_map<std::string, std::vector<RdbDataItem>>& dataMap)
{
    if (!MOC::impl) {
        return false;
    }
    return MOC::impl->QueryAllData(dataMap);
}
bool AppDomainVerifyRdbDataManager::CreateTable()
{
    if (!MOC::impl) {
        return false;
    }
    return MOC::impl->CreateTable();
}
AppDomainVerifyRdbDataManager::AppDomainVerifyRdbDataManager(const AppDomainVerifyRdbConfig& rdbConfig)
{
}
AppDomainVerifyRdbDataManager::~AppDomainVerifyRdbDataManager()
{
}
std::shared_ptr<NativeRdb::RdbStore> AppDomainVerifyRdbDataManager::GetRdbStore()
{
    return nullptr;
}
void AppDomainVerifyRdbDataManager::DelayCloseRdbStore()
{
}
bool AppDomainVerifyRdbDataManager::CheckRdbReturnIfOk(int errcode)
{
    return false;
}
bool AppDomainVerifyRdbDataManager::CheckRdbStoreExist(const std::shared_ptr<NativeRdb::RdbStore>& rdbStore)
{
    return false;
}

bool AppDomainVerifyRdbDataManager::QueryDomainByBundleName(
    const std::string& bundleName, std::vector<RdbDataItem>& items)
{
    return false;
}

bool AppDomainVerifyRdbDataManager::QueryBundleNameByDomain(const std::string& domain, std::vector<RdbDataItem>& items)
{
    return false;
}
}
