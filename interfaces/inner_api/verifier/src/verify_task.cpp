/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "verify_task.h"
#include "domain_verifier.h"
#include "app_domain_verify_hilog.h"
#include "bundle_verify_status_info.h"
#include "agent_constants.h"
#include "domain_url_util.h"

namespace OHOS {
namespace AppDomainVerify {
const std::string HTTPS = "https";
const std::set<std::string> SCHEME_WHITE_SET = { HTTPS };
const std::string FUZZY_HOST_START = "*.";

void VerifyTask::OnPostVerify(const std::string& uri, const OHOS::NetStack::HttpClient::HttpClientResponse& response)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    auto status = DomainVerifier::VerifyHost(response.GetResponseCode(), response.GetResult(), appVerifyBaseInfo_);
    uriVerifyMap_.insert_or_assign(uri, status);
    VERIFY_RESULT_EVENT(appVerifyBaseInfo_.appIdentifier, appVerifyBaseInfo_.bundleName, type_, status);
}
void VerifyTask::OnSaveVerifyResult()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = appVerifyBaseInfo_.appIdentifier;
    verifyResultInfo.hostVerifyStatusMap = uriVerifyMap_;
    if (!SaveDomainVerifyStatus(appVerifyBaseInfo_.bundleName, verifyResultInfo)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "SaveVerifyResult failed");
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}
bool VerifyTask::OnPreRequest(OHOS::NetStack::HttpClient::HttpClientRequest& request, const std::string& uri)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    request.SetURL(uri + ApplinkingAssetKeys::ASSET_PATH + ApplinkingAssetKeys::ASSET_NAME);
    request.SetMethod("GET");
    return true;
}
OHOS::AppDomainVerify::TaskType VerifyTask::GetType()
{
    return type_;
}
const std::unordered_map<std::string, InnerVerifyStatus>& VerifyTask::GetUriVerifyMap()
{
    return uriVerifyMap_;
}
void VerifyTask::InitUriVerifyMap(const std::vector<SkillUri>& skillUris)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
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
        uriVerifyMap_.insert(make_pair(it->scheme + "://" + host, InnerVerifyStatus::UNKNOWN));
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}
VerifyTask::VerifyTask(OHOS::AppDomainVerify::TaskType type, const AppVerifyBaseInfo& appVerifyBaseInfo,
    const std::vector<SkillUri>& skillUris)
    : type_(type),
      appVerifyBaseInfo_(appVerifyBaseInfo)
{
    InitUriVerifyMap(skillUris);
}
OHOS::AppDomainVerify::TaskType& VerifyTask::GetTaskType()
{
    return type_;
}
AppVerifyBaseInfo& VerifyTask::GetAppVerifyBaseInfo()
{
    return appVerifyBaseInfo_;
}
std::unordered_map<std::string, InnerVerifyStatus>& VerifyTask::GetInnerUriVerifyMap()
{
    return uriVerifyMap_;
}
bool VerifyTask::SaveDomainVerifyStatus(const std::string& bundleName, const VerifyResultInfo& verifyResultInfo)
{
    return AppDomainVerifyMgrClient::GetInstance()->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
}
}
}