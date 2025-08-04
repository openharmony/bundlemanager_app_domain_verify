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
#include <tuple>
#include <vector>
#include "app_details_data_mgr.h"
#include "ipc_skeleton.h"
#include "app_domain_verify_mgr_service.h"
#include "system_ability_definition.h"
#include "domain_url_util.h"
#include "app_domain_verify_agent_client.h"
#include "app_domain_verify_error.h"
#include "bundle_info_query.h"
#include "sa_interface/app_domain_verify_mgr_service.h"

namespace OHOS {
namespace AppDomainVerify {
constexpr const char* GET_DOMAIN_VERIFY_INFO = "ohos.permission.GET_APP_DOMAIN_BUNDLE_INFO";
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(new (std::nothrow) AppDomainVerifyMgrService());
const std::string HTTPS = "https";
const std::set<std::string> SCHEME_WHITE_SET = { HTTPS };
const std::string FUZZY_HOST_START = "*.";

AppDomainVerifyMgrService::AppDomainVerifyMgrService() : SystemAbility(APP_DOMAIN_VERIFY_MANAGER_SA_ID, true)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "new instance create.");
    appDetailsDataMgr_ = std::make_shared<AppDetailsDataMgr>();
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
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
        return;
    }
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.appIdentifier = appIdentifier;
    appVerifyBaseInfo.bundleName = bundleName;
    appVerifyBaseInfo.fingerprint = fingerprint;

    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = appIdentifier;

    CollectDomains(skillUris, verifyResultInfo);
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(bundleName, verifyResultInfo);
    AppDomainVerifyAgentClient::GetInstance()->SingleVerify(appVerifyBaseInfo, verifyResultInfo);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
}

bool AppDomainVerifyMgrService::ClearDomainVerifyStatus(const std::string& appIdentifier, const std::string& bundleName)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called, bundlename:%{public}s", bundleName.c_str());
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
        return false;
    }
    bool res = DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->DeleteVerifyStatus(bundleName);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return res;
}

bool AppDomainVerifyMgrService::FilterAbilities(const OHOS::AAFwk::Want& want,
    const std::vector<OHOS::AppExecFwk::AbilityInfo>& originAbilityInfos,
    std::vector<OHOS::AppExecFwk::AbilityInfo>& filteredAbilityInfos)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
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
        if (DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->GetVerifyStatus(
                it->bundleName, verifyResultInfo)) {
            auto itr = verifyResultInfo.hostVerifyStatusMap.find(hostVerifyKey);
            if (itr != verifyResultInfo.hostVerifyStatusMap.end() &&
                std::get<0>(itr->second) == InnerVerifyStatus::STATE_SUCCESS) {
                filteredAbilityInfos.emplace_back(*it);
            }
        }
    }
    if (filteredAbilityInfos.empty()) {
        !IsUrlInBlackList(hostVerifyKey) ?
            deferredLinkMgr_->PutDeferredLink(
                { .domain = hostVerifyKey, .url = uriString, .timeStamp = GetSecondsSince1970ToNow() }) :
            void();
    } else {
        // Calling openlink may call FilterAbilities multiple times, the link will be stored when matching default
        // browser's abilities, so remove it if the link matches verified abilities.
        deferredLinkMgr_->RemoveDeferredLink({ .domain = hostVerifyKey, .url = uriString });
    }

    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return true;
}

bool AppDomainVerifyMgrService::QueryDomainVerifyStatus(
    const std::string& bundleName, DomainVerifyStatus& domainVerificationState)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
        return false;
    }
    VerifyResultInfo verifyResultInfo;
    bool res = DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->GetVerifyStatus(bundleName, verifyResultInfo);
    domainVerificationState = DomainVerifyStatus::STATE_NONE;
    for (auto it = verifyResultInfo.hostVerifyStatusMap.begin();
         res && it != verifyResultInfo.hostVerifyStatusMap.end(); ++it) {
        if (std::get<0>(it->second) == InnerVerifyStatus::STATE_SUCCESS) {
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
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
        return false;
    }
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_ =
        DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->GetAllVerifyStatus();
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return true;
}

bool AppDomainVerifyMgrService::SaveDomainVerifyStatus(
    const std::string& bundleName, const VerifyResultInfo& verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
        return false;
    }
    bool res = DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->UpdateVerifyStatus(
        bundleName, verifyResultInfo);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call end");
    return res;
}
bool AppDomainVerifyMgrService::IsAtomicServiceUrl(const std::string& url)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
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
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
        return;
    }
    AppDomainVerifyAgentClient::GetInstance()->ConvertToExplicitWant(implicitWant, callback);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
}

void AppDomainVerifyMgrService::UpdateWhiteListUrls(const std::vector<std::string>& urls)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
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
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    auto ret = CheckPermission();
    if (ret != ErrorCode::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "CheckPermission failed:%{public}d", ret);
        return ret;
    }
    return DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->QueryAssociatedDomains(bundleName, domains) ?
        E_OK :
        E_INTERNAL_ERR;
}
int AppDomainVerifyMgrService::QueryAssociatedBundleNames(
    const std::string& domain, std::vector<std::string>& bundleNames)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    auto ret = CheckPermission();
    if (ret != ErrorCode::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "CheckPermission failed:%{public}d", ret);
        return ret;
    }
    return DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->QueryAssociatedBundleNames(domain, bundleNames) ?
        E_OK :
        E_INTERNAL_ERR;
}

int AppDomainVerifyMgrService::QueryAppDetailsWant(const std::string& url, AAFwk::Want& want)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
        return AppDetailsCode::QUERY_FAIL;
    }
    std::string bundleName;
    auto ret = appDetailsDataMgr_->QueryAppDetailsWant(url, want, bundleName);
    if (ret == AppDetailsCode::QUERY_SUCC) {
        VerifyResultInfo info;
        if (DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->GetVerifyStatus(bundleName, info)) {
            return AppDetailsCode::QUERY_FAIL;
        }
        return AppDetailsCode::QUERY_SUCC;
    }
    return AppDetailsCode::QUERY_FAIL;
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
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "DumpAllVerifyInfos fail to query all domain verify status");
        return;
    }
    for (const auto& bundleVerifyStatusInfo : allBundleVerifyStatusInfo.bundleVerifyStatusInfoMap_) {
        dumpString.append(bundleVerifyStatusInfo.first + ":\n");
        auto verifyResultInfo = bundleVerifyStatusInfo.second;
        dumpString.append("  appIdentifier:" + verifyResultInfo.appIdentifier);
        dumpString.append("\n");
        dumpString.append("  domain verify status:\n");
        for (const auto& hostVerifyStatus : verifyResultInfo.hostVerifyStatusMap) {
            dumpString.append(
                "    " + hostVerifyStatus.first + ":" + InnerVerifyStatusMap[std::get<0>(hostVerifyStatus.second)]);
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
        return ErrorCode::E_PERMISSION_DENIED;
    }

    if (!PermissionManager::IsSystemAppCall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "IsSystemAppCall failed .");
        return ErrorCode::E_IS_NOT_SYS_APP;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "CheckPermission ok .");
    return ErrorCode::E_OK;
}
void AppDomainVerifyMgrService::CollectDomains(
    const std::vector<SkillUri>& skillUris, VerifyResultInfo& verifyResultInfo)
{
    for (auto it = skillUris.begin(); it != skillUris.end(); ++it) {
        if (it->scheme.empty() || it->host.empty() || !UrlUtil::IsValidAppDomainVerifyHost(it->host) ||
            SCHEME_WHITE_SET.find(it->scheme) == SCHEME_WHITE_SET.end()) {
            APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "invalid skillUri skip.");
            continue;
        }

        std::string host = it->host;
        if (it->host.substr(0, FUZZY_HOST_START.size()) == FUZZY_HOST_START) {
            // Hosts with *.
            host = it->host.substr(FUZZY_HOST_START.size());
        }
        // validUris remove duplicates
        auto uri = it->scheme + "://" + host;
        verifyResultInfo.hostVerifyStatusMap.insert(
            make_pair(uri, std::make_tuple(InnerVerifyStatus::UNKNOWN, std::string(), 0)));
    }
}
int AppDomainVerifyMgrService::QueryVerifiedBundleWithDomains(
    std::string& bundleName, std::vector<std::string>& domains)
{
    if (!BundleInfoQuery::GetBundleNameForUid(IPCSkeleton::GetCallingUid(), bundleName)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get caller's bundleName error");
        return ErrorCode::E_INTERNAL_ERR;
    }
    if (bundleName.empty()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "can not get caller's bundleName");
        return ErrorCode::E_PARAM_ERROR;
    }
    std::string appIdentifier;
    std::string fingerPrint;
    if (!BundleInfoQuery::GetBundleInfo(bundleName, appIdentifier, fingerPrint)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get appIdentifier error");
        return ErrorCode::E_INTERNAL_ERR;
    }
    if (appIdentifier.empty()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "can not get caller's appIdentifier");
        return ErrorCode::E_PARAM_ERROR;
    }
    VerifyResultInfo verifyResultInfo;
    if (!DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->GetVerifyStatus(bundleName, verifyResultInfo)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "can not get verifyResultInfo");
        return ErrorCode::E_INTERNAL_ERR;
    }
    if (verifyResultInfo.appIdentifier != appIdentifier) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "bundle's appIdentifier not match");
        return ErrorCode::E_PARAM_ERROR;
    }
    std::for_each(std::begin(verifyResultInfo.hostVerifyStatusMap), std::end(verifyResultInfo.hostVerifyStatusMap),
        [&domains](const auto& item) {
            if (std::get<0>(item.second) == InnerVerifyStatus::STATE_SUCCESS) {
                domains.push_back(item.first);
            }
        });
    return ErrorCode::E_OK;
}

int AppDomainVerifyMgrService::GetDeferredLink(std::string& link)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    std::string bundleName;
    std::vector<std::string> domains;
    auto ret = QueryVerifiedBundleWithDomains(bundleName, domains);
    if (ret != ErrorCode::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "QueryVerifiedBundleWithDomains error:%{public}d.", ret);
        return ret;
    }
    if (domains.empty()) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "domains empty, will return.");
        return ErrorCode::E_OK;
    }
    link = deferredLinkMgr_->GetDeferredLink(bundleName, domains);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "get link, %{private}s.", link.c_str());
    return ErrorCode::E_OK;
}

bool AppDomainVerifyMgrService::IsUrlInBlackList(const std::string& url)
{
    return IsAtomicServiceUrl(url);
}

bool AppDomainVerify::AppDomainVerifyMgrService::IsShortUrl(const std::string& url)
{
    return IsAtomicServiceUrl(url);
}
void AppDomainVerifyMgrService::ConvertFromShortUrl(Want& originWant, sptr<IConvertCallback>& callback)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsSACall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only sa can call");
        return;
    }
    AppDomainVerifyAgentClient::GetInstance()->ConvertToExplicitWant(originWant, callback);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
}

bool AppDomainVerifyMgrService::QueryAbilityInfos(
    const std::string& url, bool withDefault, std::vector<AbilityInfo>& abilityInfos, bool& findDefaultApp)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    if (!PermissionManager::IsAgentCall()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "only agenet can call");
        return false;
    }
    return BundleInfoQuery::QueryAbilityInfos(url, withDefault, abilityInfos, findDefaultApp);
}
}  // namespace AppDomainVerify
}  // namespace OHOS
