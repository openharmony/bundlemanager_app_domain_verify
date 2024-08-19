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

#include <memory>
#include <string>
#include <vector>
#include "app_domain_verify_mgr_service.h"
#include "system_ability_definition.h"
#include "domain_url_util.h"
#include "app_domain_verify_agent_client.h"
#include "comm_define.h"
namespace OHOS {
namespace AppDomainVerify {
constexpr const char* GET_DOMAIN_VERIFY_INFO = "ohos.permission.GET_APP_DOMAIN_BUNDLE_INFO";
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(new AppDomainVerifyMgrService());

AppDomainVerifyMgrService::AppDomainVerifyMgrService() : SystemAbility(APP_DOMAIN_VERIFY_MANAGER_SA_ID, true)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "new instance create.");
    dataManager_ = std::make_shared<AppDomainVerifyDataMgr>();
}
AppDomainVerifyMgrService::~AppDomainVerifyMgrService()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "instance dead.");
}

void AppDomainVerifyMgrService::VerifyDomain(const std::string& appIdentifier, const std::string& bundleName,
    const std::string& fingerprint, const std::vector<SkillUri>& skillUris)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s only sa can call", __func__);
        return;
    }
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.appIdentifier = appIdentifier;
    appVerifyBaseInfo.bundleName = bundleName;
    appVerifyBaseInfo.fingerprint = fingerprint;
    AppDomainVerifyAgentClient::GetInstance()->SingleVerify(appVerifyBaseInfo, skillUris);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
}

bool AppDomainVerifyMgrService::ClearDomainVerifyStatus(const std::string& appIdentifier, const std::string& bundleName)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s only sa can call", __func__);
        return false;
    }
    bool res = dataManager_->DeleteVerifyStatus(bundleName);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return res;
}

bool AppDomainVerifyMgrService::FilterAbilities(const OHOS::AAFwk::Want& want,
    const std::vector<OHOS::AppExecFwk::AbilityInfo>& originAbilityInfos,
    std::vector<OHOS::AppExecFwk::AbilityInfo>& filtedAbilityInfos)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s only sa can call", __func__);
        return false;
    }
    if (!IsWantImplicit(want)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "want is not implicit.");
        return false;
    }

    std::string uriString = want.GetUriString();
    std::string scheme = UrlUtil::GetScheme(uriString);
    std::string host = UrlUtil::GetHost(uriString);
    if (scheme.empty() || host.empty()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "uriString is not valid.");
        return false;
    }
    std::string hostVerifyKey = scheme + "://" + host;
    for (auto it = originAbilityInfos.begin(); it != originAbilityInfos.end(); ++it) {
        // todo bms AbilityInfo contains appIdentifier
        VerifyResultInfo verifyResultInfo;
        // get from emory variable, non-IO operation.
        if (dataManager_->GetVerifyStatus(it->bundleName, verifyResultInfo)) {
            auto itr = verifyResultInfo.hostVerifyStatusMap.find(hostVerifyKey);
            if (itr != verifyResultInfo.hostVerifyStatusMap.end() && itr->second == InnerVerifyStatus::STATE_SUCCESS) {
                filtedAbilityInfos.emplace_back(*it);
            }
        }
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return true;
}

bool AppDomainVerifyMgrService::QueryDomainVerifyStatus(
    const std::string& bundleName, DomainVerifyStatus& domainVerificationState)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s only sa can call", __func__);
        return false;
    }
    VerifyResultInfo verifyResultInfo;
    bool res = dataManager_->GetVerifyStatus(bundleName, verifyResultInfo);
    domainVerificationState = DomainVerifyStatus::STATE_NONE;
    for (auto it = verifyResultInfo.hostVerifyStatusMap.begin();
         res && it != verifyResultInfo.hostVerifyStatusMap.end(); ++it) {
        if (it->second == InnerVerifyStatus::STATE_SUCCESS) {
            domainVerificationState = DomainVerifyStatus::STATE_VERIFIED;
            break;
        }
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return res;
}

bool AppDomainVerifyMgrService::QueryAllDomainVerifyStatus(BundleVerifyStatusInfo& bundleVerifyStatusInfo)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s only sa can call", __func__);
        return false;
    }
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_ = dataManager_->GetAllVerifyStatus();
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return true;
}

bool AppDomainVerifyMgrService::SaveDomainVerifyStatus(
    const std::string& bundleName, const VerifyResultInfo& verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s only sa can call", __func__);
        return false;
    }
    bool res = dataManager_->SaveVerifyStatus(bundleName, verifyResultInfo);
    if (res) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE,
            "package verify result bundleName:%{public}s\n%{public}s", bundleName.c_str(),
            verifyResultInfo.Dump().c_str());
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return res;
}
bool AppDomainVerifyMgrService::IsAtomicServiceUrl(const std::string& url)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s only sa can call", __func__);
        return false;
    }
    if (!InitConfigMgr()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "InitConfigMgr failed.");
        return false;
    }
    return whiteListConfigMgr_->IsInWhiteList(url);
}
void AppDomainVerifyMgrService::ConvertToExplicitWant(OHOS::AAFwk::Want& implicitWant, sptr<IConvertCallback>& callback)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s only sa can call", __func__);
        return;
    }
    AppDomainVerifyAgentClient::GetInstance()->ConvertToExplicitWant(implicitWant, callback);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
}

void AppDomainVerifyMgrService::UpdateWhiteListUrls(const std::vector<std::string>& urls)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s only sa can call", __func__);
        return;
    }
    if (!InitConfigMgr()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "InitConfigMgr failed.");
        return;
    }
    std::unordered_set<std::string> whiteList(urls.begin(), urls.end());
    whiteListConfigMgr_->UpdateWhiteList(whiteList);
}

int AppDomainVerifyMgrService::QueryAssociatedDomains(const std::string& bundleName, std::vector<std::string>& domains)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    auto ret = CheckPermission();
    if (ret != CommonErrorCode::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "CheckPermission failed:%{public}d", ret);
        return ret;
    }
    return dataManager_->QueryAssociatedDomains(bundleName, domains) ? E_OK : E_INTERNAL_ERR;
}
int AppDomainVerifyMgrService::QueryAssociatedBundleNames(
    const std::string& domain, std::vector<std::string>& bundleNames)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    auto ret = CheckPermission();
    if (ret != CommonErrorCode::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "CheckPermission failed:%{public}d", ret);
        return ret;
    }
    return dataManager_->QueryAssociatedBundleNames(domain, bundleNames) ? E_OK : E_INTERNAL_ERR;
}

bool AppDomainVerifyMgrService::IsWantImplicit(const OHOS::AAFwk::Want& want)
{
    auto element = want.GetElement();
    std::string bundleName = element.GetBundleName();
    std::string abilityName = element.GetAbilityName();
    // want is explicit query
    if (!bundleName.empty() && !abilityName.empty()) {
        return false;
    }
    return true;
}

// sa_main进程统一调用
void AppDomainVerifyMgrService::OnStart()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "OnStart");
    bool res = Publish(this);
    if (!res) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "Publish failed");
    }
}

void AppDomainVerifyMgrService::OnStop()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
}
void AppDomainVerifyMgrService::OnDump()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnDump");
}

int AppDomainVerifyMgrService::Dump(int fd, const std::vector<std::u16string>& args)
{
    std::string dumpString{};
    DumpAllVerifyInfos(dumpString);
    (void)write(fd, dumpString.c_str(), dumpString.size());
    return 0;
}
void AppDomainVerifyMgrService::DumpAllVerifyInfos(std::string& dumpString)
{
    BundleVerifyStatusInfo allBundleVerifyStatusInfo;
    if (!QueryAllDomainVerifyStatus(allBundleVerifyStatusInfo)) {
        return;
    }
    for (const auto& bundleVerifyStatusInfo : allBundleVerifyStatusInfo.bundleVerifyStatusInfoMap_) {
        dumpString.append(bundleVerifyStatusInfo.first + ":\n");
        auto verifyResultInfo = bundleVerifyStatusInfo.second;
        dumpString.append("  appIdentifier:" + verifyResultInfo.appIdentifier);
        dumpString.append("\n");
        dumpString.append("  domain verify status:\n");
        for (const auto& hostVerifyStatus : verifyResultInfo.hostVerifyStatusMap) {
            dumpString.append("    " + hostVerifyStatus.first + ":" + InnerVerifyStatusMap[hostVerifyStatus.second]);
            dumpString.append("\n");
        }
    }
}
bool AppDomainVerifyMgrService::InitConfigMgr()
{
    if (whiteListConfigMgr_ != nullptr) {
        return true;
    }
    std::lock_guard<std::mutex> lock(initConfigMutex_);
    if (whiteListConfigMgr_ == nullptr) {
        whiteListConfigMgr_ = std::make_shared<WhiteListConfigMgr>();
    }
    if (whiteListConfigMgr_ == nullptr) {
        return false;
    }
    return true;
}
int AppDomainVerifyMgrService::CheckPermission()
{
    if (!PermissionManager::CheckPermission(GET_DOMAIN_VERIFY_INFO)) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "CheckPermission failed %{public}s.", GET_DOMAIN_VERIFY_INFO);
        return CommonErrorCode::E_PERMISSION_DENIED;
    }

    if (!PermissionManager::IsSystemAppCall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "IsSystemAppCall failed .");
        return CommonErrorCode::E_IS_NOT_SYS_APP;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "CheckPermission ok .");
    return CommonErrorCode::E_OK;
}
}  // namespace AppDomainVerify
}  // namespace OHOS
