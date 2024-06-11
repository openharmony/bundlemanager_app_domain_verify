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

#include "bms/bundle_info_query.h"
#include "bundle_mgr_client.h"
#include "bundle_info.h"
#include "os_account_manager.h"
#include "app_domain_verify_hilog.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "bundlemgr/bundle_mgr_proxy.h"

namespace OHOS {
namespace AppDomainVerify {
bool BundleInfoQuery::GetBundleInfo(const std::string &bundleName, std::string &appIdentifier, std::string &fingerprint)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    if (bundleMgrProxy == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "bundleMgrProxy is nullptr.");
        return false;
    }
    int32_t userId = GetCurrentAccountId();
    if (userId < 0) {
        return false;
    }
    OHOS::AppExecFwk::BundleInfo bundleInfo;
    auto ret = bundleMgrProxy->GetBundleInfoV9(bundleName,
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_SIGNATURE_INFO), bundleInfo, userId);
    if (ret != ERR_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "GetBundleInfo failed, ret: %{public}d.", ret);
        return false;
    }
    appIdentifier = bundleInfo.signatureInfo.appIdentifier;
    fingerprint = bundleInfo.signatureInfo.fingerprint;
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
    return true;
}

sptr<AppExecFwk::IBundleMgr> BundleInfoQuery::GetBundleMgrProxy()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE,
            "GetBundleMgrProxy, systemAbilityManager is null");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "GetBundleMgrProxy, remoteObject is null");
        return nullptr;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}

int32_t BundleInfoQuery::GetCurrentAccountId()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    std::vector<int32_t> osAccountIds;
    ErrCode ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(osAccountIds);
    if (ret != ERR_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "QueryActiveOsAccountIds failed.");
        return -1;
    }

    if (osAccountIds.empty()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "osAccountInfos is empty, no accounts.");
        return -1;
    }

    auto iter = std::find_if(osAccountIds.cbegin(), osAccountIds.cend(),
        [](const int32_t &accountId) { return accountId >= 0; });
    if (iter != osAccountIds.end()) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
        return *iter;
    }
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE,
        "GetCurrentAccountId failed, no osAccountIds now.");
    return -1;
}
}
}