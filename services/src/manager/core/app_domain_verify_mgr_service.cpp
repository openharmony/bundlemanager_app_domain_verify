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

#include <iostream>
#include <memory>

#include <sstream>
#include <string>
#include <vector>

#include "app_domain_verify_mgr_service.h"
#include "iremote_broker.h"
#include "system_ability_definition.h"
#include "manager_constants.h"
#include "domain_url_util.h"
#include "bundle_verify_status_info.h"
#include "app_verify_base_info.h"
#include "iservice_registry.h"
#include "app_domain_verify_agent_client.h"

namespace OHOS {
namespace AppDomainVerify {

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(new AppDomainVerifyMgrService());

AppDomainVerifyMgrService::AppDomainVerifyMgrService()
    : SystemAbility(APP_DOMAIN_VERIFY_MANAGER_SA_ID, true)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "new instance create.");
    dataManager_ = std::make_shared<AppDomainVerifyDataMgr>();
}
AppDomainVerifyMgrService::~AppDomainVerifyMgrService()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "instance dead.");
}

void AppDomainVerifyMgrService::VerifyDomain(const std::string &appIdentifier, const std::string &bundleName,
    const std::string &fingerprint, const std::vector<SkillUri> &skillUris)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.appIdentifier = appIdentifier;
    appVerifyBaseInfo.bundleName = bundleName;
    appVerifyBaseInfo.fingerprint = fingerprint;
    AppDomainVerifyAgentClient::GetInstance()->SingleVerify(appVerifyBaseInfo, skillUris);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
}

bool AppDomainVerifyMgrService::ClearDomainVerifyStatus(const std::string &appIdentifier, const std::string &bundleName)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    bool res = dataManager_->DeleteVerifyStatus(bundleName);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return res;
}

bool AppDomainVerifyMgrService::FilterAbilities(const OHOS::AAFwk::Want &want,
    const std::vector<OHOS::AppExecFwk::AbilityInfo> &originAbilityInfos,
    std::vector<OHOS::AppExecFwk::AbilityInfo> &filtedAbilityInfos)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
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
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyMgrService::QueryDomainVerifyStatus(const std::string &bundleName,
    DomainVerifyStatus &domainVerificationState)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
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
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return res;
}

bool AppDomainVerifyMgrService::QueryAllDomainVerifyStatus(BundleVerifyStatusInfo &bundleVerifyStatusInfo)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_ = dataManager_->GetAllVerifyStatus();
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return true;
}

bool AppDomainVerifyMgrService::SaveDomainVerifyStatus(const std::string &bundleName,
    const VerifyResultInfo &verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    bool res = dataManager_->SaveVerifyStatus(bundleName, verifyResultInfo);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return res;
}

bool AppDomainVerifyMgrService::IsWantImplicit(const OHOS::AAFwk::Want &want)
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
    AppDomainVerifyMgrServiceStub::PostDelayUnloadTask();
    bool res = Publish(this);
    if (!res) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "Publish failed");
    }
}

void AppDomainVerifyMgrService::OnStop()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
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
}  // namespace AppDomainVerify
}  // namespace OHOS
