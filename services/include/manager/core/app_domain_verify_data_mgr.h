/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#ifndef APP_DOMAIN_VERIFY_DATA_MGR_H
#define APP_DOMAIN_VERIFY_DATA_MGR_H

#include <memory>
#include <string>
#include <vector>

#include "app_domain_verify_hilog.h"
#include "app_domain_verify_rdb_data_manager.h"
#include "inner_verify_status.h"
#include "singleton.h"
#include "bundle_verify_status_info.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyDataMgr {
public:
    AppDomainVerifyDataMgr();
    virtual ~AppDomainVerifyDataMgr();

    /**
     * @brief Get AppDomainVerify status.
     * @param bundleName Indicates the bundleName.
     * @param verifyResultInfo output value.
     * @return Returns true if this function is successfully called; returns false
     * otherwise.
     */
    bool GetVerifyStatus(const std::string& bundleName, VerifyResultInfo& verifyResultInfo);

    /**
     * @brief insert the AppDomainVerify status.
     * @param bundleName Indicates the bundleName.
     * @param verifyResultInfo Indicates the value to save.
     * @return Returns true if this function is successfully called; returns false
     * otherwise.
     */
    bool InsertVerifyStatus(const std::string& bundleName, const VerifyResultInfo& verifyResultInfo);

    /**
     * @brief udapte the AppDomainVerify status.
     * @param bundleName Indicates the bundleName.
     * @param verifyResultInfo Indicates the value to save.
     * @return Returns true if this function is successfully called; returns false
     * otherwise.
     */
    bool UpdateVerifyStatus(const std::string& bundleName, const VerifyResultInfo& verifyResultInfo);

    /**
     * @brief Delete the AppDomainVerify status.
     * @param bundleName Indicates the bundleName.
     * @return Returns true if this function is successfully called; returns false
     * otherwise.
     */
    bool DeleteVerifyStatus(const std::string& bundleName);

    /**
     * @brief Get all AppDomainVerify status.
     * @return Returns verifyMap_ const ref
     * otherwise.
     */
    const std::unordered_map<std::string, VerifyResultInfo>& GetAllVerifyStatus();
    bool QueryAssociatedDomains(const std::string& bundleName, std::vector<std::string>& domains);
    bool QueryAssociatedBundleNames(const std::string& domain, std::vector<std::string>& bundleNames);

private:
    bool GetParamKey(const std::string& bundleName, std::string& paramKey);
    void UpdateVerifyMap(const std::string& bundleName, const VerifyResultInfo& verifyResultInfo);
    bool InitRdb();
    bool LoadAllFromRdb();
    bool VerifyResultInfoToDB(const std::string bundleName, const VerifyResultInfo& verifyResultInfo);
    bool DBToVerifyResultInfo(const std::vector<RdbDataItem>& items, VerifyResultInfo& verifyResultInfo);
    bool LoadData();

private:
    std::shared_ptr<AppDomainVerifyRdbDataManager> rdbDataManager_ = nullptr;
    std::mutex verifyMapMutex_;
    std::shared_ptr<std::unordered_map<std::string, VerifyResultInfo>> verifyMap_ = nullptr;
};
}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_DATA_MGR_H