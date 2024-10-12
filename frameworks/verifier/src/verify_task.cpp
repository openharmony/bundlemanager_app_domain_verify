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
#include <cstdint>
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
const static int CLIENT_ERR_MAX_RETRY_COUNTS = 7; // 7 times for max retry count
const static int CLIENT_ERR_BASE_RETRY_DURATION_S = 3600; // 1h for base duration
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
    staHandlerMap[STATE_SUCCESS] = [this](std::string time, int cnt)->bool {
        return HandleStateSuccess(time, cnt);
    };
    staHandlerMap[FAILURE_CLIENT_ERROR] = [this](std::string time, int cnt)->bool {
        return HandleFailureClientError(time, cnt);
    };
    staHandlerMap[FORBIDDEN_FOREVER] = [this](std::string time, int cnt)->bool {
        return HandleForbiddenForever(time, cnt);
    };
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
    auto iter = staHandlerMap.find(status);
    if (iter != staHandlerMap.end()) {
        return iter->second(verifyTime, verifyCnt);
    }
    return true;
}

int64_t VerifyTask::CalcRetryDuration(int verifyCnt)
{
    int64_t duration = pow(2, verifyCnt) * CLIENT_ERR_BASE_RETRY_DURATION_S; // base * 2 ^ verifyCnt
    return duration;
}

bool VerifyTask::HandleFailureClientError(std::string verifyTime, int verifyCnt)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "called");
    if (!verifyTime.empty()) {
        int64_t currTs = GetSecondsSince1970ToNow();
        int64_t lastTs = static_cast<int64_t>(std::stoll(verifyTime));
        int64_t duration = currTs - lastTs;
        int64_t currRetryDuration = CalcRetryDuration(verifyCnt);
        if (duration <= currRetryDuration) {
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_EXTENSION,
                "last time:%{public}s, curr time:%{public}s, "
                "duration:%{public}s "
                "is less than max retry duration:%{public}s, not retry",
                std::to_string(lastTs).c_str(),
                std::to_string(currTs).c_str(),
                std::to_string(duration).c_str(),
                std::to_string(currRetryDuration).c_str());
            return false;
        }
    }
    return true;
}

bool VerifyTask::HandleStateSuccess(std::string verifyTime, int verifyCnt)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "called");
    return false;
}

bool VerifyTask::HandleForbiddenForever(std::string verifyTime, int verifyCnt)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "called");
    return false;
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
        hostVerifyStatusMap.insert_or_assign(uri, std::make_tuple(verifyStatus, verifyTs, verifyCnt));
        return;
    }
    if (verifyStatus == InnerVerifyStatus::FAILURE_CLIENT_ERROR) {
        std::tie(std::ignore, std::ignore, verifyCnt) = iter->second;
        verifyCnt++;
        if (verifyCnt >= CLIENT_ERR_MAX_RETRY_COUNTS) {
            verifyStatus = InnerVerifyStatus::FORBIDDEN_FOREVER;
        }
    }
    std::get<0>(iter->second) = verifyStatus;
    std::get<1>(iter->second) = verifyTs;
    std::get<2>(iter->second) = verifyCnt; // 2 is cnt
}

}
}