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

#include <deque>
#include <memory>
#include <string>
#include "app_domain_verify_task_mgr.h"
#include "app_domain_verify_hilog.h"
#include "http_client.h"
#include "domain_verifier.h"
#include "bundle_verify_status_info.h"
#include "app_domain_verify_mgr_client.h"
#include "agent_constants.h"

namespace OHOS {
namespace AppDomainVerify {
using namespace OHOS::NetStack::HttpClient;
const int URI_IN_TASK_ONCE_REQUEST_SIZE = 1;

AppDomainVerifyTaskMgr::AppDomainVerifyTaskMgr()
{
    httpClientTaskFactory_ = std::make_unique<VerifyHttpTaskFactory>();
}

bool AppDomainVerifyTaskMgr::AddTask(const std::shared_ptr<Task> &task)
{
    if (task == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "input task is null!");
        return false;
    }
    if (task->type_ >= TaskType::UNKNOWN_TASK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "input task type error: %d", task->type_);
        return false;
    }
    {
        std::unique_lock<std::shared_mutex> lock(queueMutex);
        taskQueue.push_back(task);
    }
    Run();
    return true;
}

void AppDomainVerifyTaskMgr::Run()
{
    if (taskRunning) {
        return;
    }
    taskRunning = true;
    std::shared_ptr<Task> verifyTask = nullptr;
    {
        std::unique_lock<std::shared_mutex> lock(queueMutex);
        if (taskQueue.empty()) {
            taskRunning = false;
            return;
        }
        verifyTask = taskQueue.front();
        taskQueue.pop_front();
    }

    if (verifyTask != nullptr) {
        std::queue<std::vector<std::string>> urisQueue;
        std::vector<std::string> uris;
        for (auto it = verifyTask->uriVerifyMap_.begin(); it != verifyTask->uriVerifyMap_.end(); ++it) {
            uris.emplace_back(it->first);
            if (uris.size() == URI_IN_TASK_ONCE_REQUEST_SIZE || std::next(it) == verifyTask->uriVerifyMap_.end()) {
                urisQueue.push(uris);
                uris.clear();
            }
        }
        HttpSessionTaskStart(verifyTask, urisQueue);
    } else {
        taskRunning = false;
        Run();
    }
}

void AppDomainVerifyTaskMgr::HttpSessionTaskStart(const std::shared_ptr<Task> &verifyTask,
    std::queue<std::vector<std::string>> urisQueue)
{
    if (urisQueue.empty()) {
        SaveVerifyResult(verifyTask->appVerifyBaseInfo_, verifyTask->uriVerifyMap_);
        taskRunning = false;
        Run();
        return;
    }
    HttpClientRequest httpClientRequest;
    std::vector<std::string> urisOnceRequest;
    urisOnceRequest.insert(urisOnceRequest.end(), urisQueue.front().begin(), urisQueue.front().end());
    urisQueue.pop();
    for (auto it = urisOnceRequest.begin(); it != urisOnceRequest.end(); ++it) {
        auto uri = *it;
        httpClientRequest.SetURL(uri + ApplinkingAssetKeys::ASSET_PATH + ApplinkingAssetKeys::ASSET_NAME);
        httpClientRequest.SetMethod("GET");

        auto httpTask = httpClientTaskFactory_->CreateTask(httpClientRequest);
        // All callbacks will execute on the same sub-thread.
        httpTask->OnSuccess([=](const HttpClientRequest &request, const HttpClientResponse &response) {
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "Task OnSuccess, retcode : %{public}d.",
                response.GetResponseCode());
            verifyTask->uriVerifyMap_.insert_or_assign(uri,
                DomainVerifier::VerifyHost(response.GetResponseCode(), response.GetResult(),
                    verifyTask->appVerifyBaseInfo_));
            HttpSessionTaskStart(verifyTask, urisQueue);
        });
        httpTask->OnFail(
            [=](const HttpClientRequest &request, const HttpClientResponse &response, const HttpClientError &error) {
                APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE,
                    "OnFail,cause: %{public}d, %{public}s.", error.GetErrorCode(), error.GetErrorMessage().c_str());
                verifyTask->uriVerifyMap_.insert_or_assign(uri, InnerVerifyStatus::FAILURE_HTTP_UNKNOWN);
                HttpSessionTaskStart(verifyTask, urisQueue);
            });

        httpTask->OnDataReceive([](const HttpClientRequest &request, const uint8_t *data, size_t length) {
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnDataReceive.");
        });
        httpTask->Start();
    }
}

void AppDomainVerifyTaskMgr::SaveVerifyResult(const AppVerifyBaseInfo &appVerifyBaseInfo,
    const std::unordered_map<std::string, InnerVerifyStatus> &uriVerifyMap)
{
    // todo
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = appVerifyBaseInfo.appIdentifier;
    verifyResultInfo.hostVerifyStatusMap = uriVerifyMap;
    if (!AppDomainVerifyMgrClient::GetInstance()->SaveDomainVerifyStatus(appVerifyBaseInfo.bundleName, verifyResultInfo)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "SaveVerifyResult failed");
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}
bool AppDomainVerifyTaskMgr::IsIdle()
{
    std::unique_lock<std::shared_mutex> lock(queueMutex);
    return taskQueue.empty() && !taskRunning;
}

}
}
