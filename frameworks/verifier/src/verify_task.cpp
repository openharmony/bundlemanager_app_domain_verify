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

#include <algorithm>
#include "verify_task.h"
#include "domain_verifier.h"
#include "app_domain_verify_hilog.h"
#include "bundle_verify_status_info.h"
#include "agent_constants.h"
#include "domain_url_util.h"
#include "app_domain_verify_task_mgr.h"
#include "verify_http_task.h"

namespace OHOS {
namespace AppDomainVerify {
const std::string HTTPS = "https";
const std::set<std::string> SCHEME_WHITE_SET = { HTTPS };
const std::string FUZZY_HOST_START = "*.";

void VerifyTask::OnPostVerify(const std::string& uri, const OHOS::NetStack::HttpClient::HttpClientResponse& response)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    auto status = DomainVerifier::VerifyHost(response.GetResponseCode(), response.GetResult(), appVerifyBaseInfo_);
    verifyResultInfo_.hostVerifyStatusMap.insert_or_assign(uri, status);
    unVerifiedSet_.erase(uri);
    if (unVerifiedSet_.empty()) {
        OnSaveVerifyResult();
    }
    VERIFY_RESULT_EVENT(appVerifyBaseInfo_.appIdentifier, appVerifyBaseInfo_.bundleName, type_, status);
}
void VerifyTask::OnSaveVerifyResult()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    if (!SaveDomainVerifyStatus(appVerifyBaseInfo_.bundleName, verifyResultInfo_)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "SaveVerifyResult failed");
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "call end");
}
bool VerifyTask::OnPreRequest(OHOS::NetStack::HttpClient::HttpClientRequest& request, const std::string& uri)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
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
    return verifyResultInfo_.hostVerifyStatusMap;
}
void VerifyTask::InitUriUnVerifySetMap(const VerifyResultInfo& verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    for (const auto& domainPair : verifyResultInfo.hostVerifyStatusMap) {
        unVerifiedSet_.insert(domainPair.first);
    }

    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "call end");
}
VerifyTask::VerifyTask(OHOS::AppDomainVerify::TaskType type, const AppVerifyBaseInfo& appVerifyBaseInfo,
    const VerifyResultInfo& verifyResultInfo)
    : type_(type), appVerifyBaseInfo_(appVerifyBaseInfo), verifyResultInfo_(verifyResultInfo)
{
    InitUriUnVerifySetMap(verifyResultInfo);
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

void VerifyTask::Execute()
{
    std::for_each(verifyResultInfo_.hostVerifyStatusMap.begin(), verifyResultInfo_.hostVerifyStatusMap.end(),
        [this](const std::pair<std::string, InnerVerifyStatus>& element) {
            auto verifyHttpTask = std::make_shared<VerifyHttpTask>(element.first, shared_from_this());
            AppDomainVerifyTaskMgr::GetInstance()->AddTask(verifyHttpTask);
        });
}
}
}