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
#include <stdint.h>
#include <tuple>
#include "app_domain_verify_mgr_client.h"
#include "datetime_ex.h"
#include "inner_verify_status.h"
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
const static int CLIENT_ERR_MAX_RETRY_COUNTS = 5; // 5 times for retry
// const static int CLIENT_ERR_BASE_RETRY_DURATION_S = 14400; // 4h for base duration
const static int CLIENT_ERR_BASE_RETRY_DURATION_S = 0; // 4h for base duration
void VerifyTask::OnPostVerify(const std::string& uri, const OHOS::NetStack::HttpClient::HttpClientResponse& response)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    auto status = DomainVerifier::VerifyHost(response.GetResponseCode(), response.GetResult(), appVerifyBaseInfo_);
    UpdateVerifyResultInfo(uri, status);
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
const HostVerifyStatusMap& VerifyTask::GetUriVerifyMap()
{
    return verifyResultInfo_.hostVerifyStatusMap;
}
void VerifyTask::InitUriUnVerifySetMap(const VerifyResultInfo& verifyResultInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "called");
    for (const auto& hostVerifyStatusInfo : verifyResultInfo.hostVerifyStatusMap) {
        if (IsNeedRetry(hostVerifyStatusInfo.second)) {
            unVerifiedSet_.insert(hostVerifyStatusInfo.first);
        }
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

bool VerifyTask::SaveDomainVerifyStatus(const std::string& bundleName, const VerifyResultInfo& verifyResultInfo)
{
    return AppDomainVerifyMgrClient::GetInstance()->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
}

void VerifyTask::Execute()
{
    for (auto& hostVerifyStatusInfo : verifyResultInfo_.hostVerifyStatusMap) {
        if (unVerifiedSet_.count(hostVerifyStatusInfo.first) != 0) {
            auto verifyHttpTask = std::make_shared<VerifyHttpTask>(hostVerifyStatusInfo.first, shared_from_this());
            AppDomainVerifyTaskMgr::GetInstance()->AddTask(verifyHttpTask);
        }
    }
}

bool VerifyTask::IsNeedRetry(const std::tuple<InnerVerifyStatus, std::string, int>& info)
{
    auto [status, verifyTime, verifyCnt] = info;
    if (status == InnerVerifyStatus::STATE_SUCCESS || status == InnerVerifyStatus::FORBIDDEN_FOREVER) {
        return false;
    }
    if (status == InnerVerifyStatus::FAILURE_CLIENT_ERROR && !verifyTime.empty()) {
        int64_t currTs = GetSecondsSince1970ToNow();
        int64_t lastTs = std::stol(verifyTime);
        int64_t duration = currTs - lastTs;
        int64_t currRetryDuration = verifyCnt * CLIENT_ERR_BASE_RETRY_DURATION_S;
        if (duration <= currRetryDuration) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "status:%d, last time:%ld, curr time:%ld, duration:%ld is less than max retry duration:%ld, not retry", status, lastTs, currTs, duration, currRetryDuration);
            return false;
        }
    }
    return true;
}


void VerifyTask::UpdateVerifyResultInfo(const std::string& uri, InnerVerifyStatus status)
{
    auto verifyStatus = status;
    auto currTs = GetSecondsSince1970ToNow();
    std::string verifyTs = std::to_string(currTs);
    int verifyCnt = 0;
    auto& hostVerifyStatusMap = verifyResultInfo_.hostVerifyStatusMap;
    auto iter = hostVerifyStatusMap.find(uri);
    if (iter == hostVerifyStatusMap.end()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "uri don't exist in hostVerifyStatusMap, uri:%{public}s", uri.c_str());
        hostVerifyStatusMap.insert_or_assign(uri, std::make_tuple(verifyStatus, verifyTs, verifyCnt));
        return;
    }
    if (verifyStatus == InnerVerifyStatus::FAILURE_CLIENT_ERROR) {
       std::tie (std::ignore, std::ignore, verifyCnt) = iter->second;
       verifyCnt++;
       if (verifyCnt >= CLIENT_ERR_MAX_RETRY_COUNTS) {
            verifyStatus = InnerVerifyStatus::FORBIDDEN_FOREVER;
       }
    }
    std::get<0>(iter->second) = verifyStatus;
    std::get<1>(iter->second) = verifyTs;
    std::get<2>(iter->second) = verifyCnt;
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "test Update verify result, uri:%{public}s, status:%{public}d, verifyTs:%{public}s, cnt:%{public}d", uri.c_str(), status, verifyTs.c_str(), verifyCnt);
}

}
}