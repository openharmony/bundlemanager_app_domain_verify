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
#include "os_account_manager.h"
#include "app_domain_verify_hilog.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "app_domain_verify_hisysevent.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace AppDomainVerify {
constexpr int USER_ID_DEFAULT = 100;
bool BundleInfoQuery::GetBundleInfo(const std::string& bundleName, std::string& appIdentifier, std::string& fingerprint)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "called");
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    if (bundleMgrProxy == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "bundleMgrProxy is nullptr.");
        UNIVERSAL_ERROR_EVENT(CONNECT_OTHER_FAULT);
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
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "GetBundleInfo failed, ret: %{public}d.", ret);
        return false;
    }
    appIdentifier = bundleInfo.signatureInfo.appIdentifier;
    fingerprint = bundleInfo.signatureInfo.fingerprint;
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "call end");
    return true;
}

sptr<AppExecFwk::IBundleMgr> BundleInfoQuery::GetBundleMgrProxy()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "called");
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "GetBundleMgrProxy, systemAbilityManager is null");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "GetBundleMgrProxy, remoteObject is null");
        return nullptr;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "call end");
    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}

int32_t BundleInfoQuery::GetCurrentAccountId()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "called");
    std::vector<int32_t> osAccountIds;
    ErrCode ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(osAccountIds);
    if (ret != ERR_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "QueryActiveOsAccountIds failed.");
        return -1;
    }

    if (osAccountIds.empty()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "osAccountInfos is empty, no accounts.");
        return -1;
    }

    auto iter = std::find_if(
        osAccountIds.cbegin(), osAccountIds.cend(), [](const int32_t& accountId) { return accountId >= 0; });
    if (iter != osAccountIds.end()) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "call end");
        return *iter;
    }
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "GetCurrentAccountId failed, no osAccountIds now.");
    return -1;
}
bool BundleInfoQuery::GetBundleNameForUid(const int uid, std::string& bundleName)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "called");
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    if (bundleMgrProxy == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "bundleMgrProxy is nullptr.");
        UNIVERSAL_ERROR_EVENT(CONNECT_OTHER_FAULT);
        return false;
    }

    // use sa identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto ret = bundleMgrProxy->GetBundleNameForUid(uid, bundleName);
    IPCSkeleton::SetCallingIdentity(identity);
    if (ret != ERR_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "GetBundleInfo failed, ret: %{public}d.", ret);
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "call end");
    return true;
}

bool BundleInfoQuery::QueryAbilityInfosByUrl(
    const std::string& bundleName, const std::string& url, std::vector<AbilityInfo>& abilityInfos)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "called");
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    if (bundleMgrProxy == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "bundleMgrProxy is nullptr.");
        UNIVERSAL_ERROR_EVENT(CONNECT_OTHER_FAULT);
        return false;
    }
    Want want;
    //    want.SetBundle(bundleName);
    want.SetUri(url);
    want.AddEntity(ENTITY_BROWSER);
    want.SetAction(ACTION_VIEW_DATA);

    // use sa identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto ret = bundleMgrProxy->QueryAbilityInfosV9(
        want, AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_DEFAULT, USER_ID_DEFAULT, abilityInfos);
    IPCSkeleton::SetCallingIdentity(identity);
    if (ret != ERR_OK) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "GetBundleInfo failed, ret: %{public}d.", ret);
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "call end");
    return true;
}
bool BundleInfoQuery::GetBundleInfosV9(const std::string& bundleName, std::vector<AbilityInfo>& abilityInfos)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "called");
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    if (bundleMgrProxy == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "bundleMgrProxy is nullptr.");
        UNIVERSAL_ERROR_EVENT(CONNECT_OTHER_FAULT);
        return false;
    }

    AppExecFwk::BundleInfo bundleInfo;
    // use sa identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto ret = bundleMgrProxy->GetBundleInfoV9(
        bundleName, AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_DEFAULT, bundleInfo, USER_ID_DEFAULT);
    IPCSkeleton::SetCallingIdentity(identity);
    if (ret != ERR_OK) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "GetBundleInfo failed, ret: %{public}d.", ret);
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "call end");
    return true;
};
}
}