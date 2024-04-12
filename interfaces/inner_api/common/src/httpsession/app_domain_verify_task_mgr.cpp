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

#include "app_domain_verify_task_mgr.h"
#include <deque>
#include <memory>
#include <string>
#include "app_domain_verify_hilog.h"

namespace OHOS {
namespace AppDomainVerify {
std::shared_ptr<AppDomainVerifyTaskMgr> AppDomainVerifyTaskMgr::instance_ = nullptr;
std::mutex AppDomainVerifyTaskMgr::mutex_;
const size_t MAX_RESPONSE_LEN = 20 * 1024;

using namespace OHOS::NetStack::HttpClient;
const int URI_IN_TASK_ONCE_REQUEST_SIZE = 1;

std::shared_ptr<AppDomainVerifyTaskMgr> AppDomainVerifyTaskMgr::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<AppDomainVerifyTaskMgr>();
        }
    }
    return instance_;
}

void AppDomainVerifyTaskMgr::DestroyInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ != nullptr) {
        instance_.reset();
        instance_ = nullptr;
    }
}

AppDomainVerifyTaskMgr::AppDomainVerifyTaskMgr()
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "AppDomainVerifyTaskMgr new!");
    httpClientTaskFactory_ = std::make_unique<VerifyHttpTaskFactory>();
}

AppDomainVerifyTaskMgr::~AppDomainVerifyTaskMgr()
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "AppDomainVerifyTaskMgr destory!");
}

bool AppDomainVerifyTaskMgr::AddTask(const std::shared_ptr<IVerifyTask>& task)
{
    if (task == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "input task is null!");
        return false;
    }
    if (task->GetType() > TaskType::SCHEDULE_REFRESH_TASK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "input task type error: %d", task->GetType());
        return false;
    }
    {
        std::unique_lock<std::shared_mutex> lock(queueMutex_);
        taskQueue_.push_back(task);
    }
    Run();
    return true;
}

void AppDomainVerifyTaskMgr::Run()
{
    if (taskRunning_) {
        return;
    }
    taskRunning_ = true;
    std::shared_ptr<IVerifyTask> verifyTask = nullptr;
    {
        std::unique_lock<std::shared_mutex> lock(queueMutex_);
        if (taskQueue_.empty()) {
            taskRunning_ = false;
            return;
        }
        verifyTask = taskQueue_.front();
        taskQueue_.pop_front();
    }

    if (verifyTask != nullptr) {
        std::queue<std::vector<std::string>> urisQueue;
        std::vector<std::string> uris;
        auto uriVerifyMap = verifyTask->GetUriVerifyMap();
        for (auto it = uriVerifyMap.begin(); it != uriVerifyMap.end(); ++it) {
            uris.emplace_back(it->first);
            if (uris.size() == URI_IN_TASK_ONCE_REQUEST_SIZE || std::next(it) == uriVerifyMap.end()) {
                urisQueue.push(uris);
                uris.clear();
            }
        }
        HttpSessionTaskStart(verifyTask, urisQueue);
    } else {
        taskRunning_ = false;
        Run();
    }
}

void AppDomainVerifyTaskMgr::HttpSessionTaskStart(
    const std::shared_ptr<IVerifyTask>& verifyTask, std::queue<std::vector<std::string>> urisQueue)
{
    if (verifyTask == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "HttpSessionTaskStart nullptr!");
        return;
    }
    if (urisQueue.empty()) {
        verifyTask->OnSaveVerifyResult();
        taskRunning_ = false;
        Run();
        return;
    }
    HttpClientRequest httpClientRequest;
    std::vector<std::string> urisOnceRequest;
    urisOnceRequest.insert(urisOnceRequest.end(), urisQueue.front().begin(), urisQueue.front().end());
    urisQueue.pop();
    for (auto it = urisOnceRequest.begin(); it != urisOnceRequest.end(); ++it) {
        auto uri = *it;
        if (!verifyTask->OnPreRequest(httpClientRequest, uri)) {
            APP_DOMAIN_VERIFY_HILOGE(
                APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnPreRequest failed %{public}s, continue.", uri.c_str());
            continue;
        }
        StartHttpTask(verifyTask, urisQueue, httpClientRequest, uri);
    }
}

void AppDomainVerifyTaskMgr::StartHttpTask(const std::shared_ptr<IVerifyTask>& verifyTask,
    std::queue<std::vector<std::string>>& urisQueue, const HttpClientRequest& httpClientRequest, const std::string& uri)
{
    auto httpTask = this->httpClientTaskFactory_->CreateTask(httpClientRequest);
    // All callbacks will execute on the same sub-thread.
    httpTask->OnSuccess([=](const HttpClientRequest& request, const HttpClientResponse& response) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "Task OnSuccess, retcode : %{public}d.",
            response.GetResponseCode());
        verifyTask->OnPostVerify(uri, response);
        this->HttpSessionTaskStart(verifyTask, urisQueue);
    });
    httpTask->OnFail(
        [=](const HttpClientRequest& request, const HttpClientResponse& response, const HttpClientError& error) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnFail,cause: %{public}d, %{public}s.",
                error.GetErrorCode(), error.GetErrorMessage().c_str());
            verifyTask->OnPostVerify(uri, response);
            this->HttpSessionTaskStart(verifyTask, urisQueue);
        });

    httpTask->OnDataReceive([httpTask](const HttpClientRequest& request, const uint8_t* data, size_t length) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnDataReceive size:%{public}zu.", length);
        if (httpTask->GetLen() + length > MAX_RESPONSE_LEN) {
            APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "recieve data exceeds, will cancel.");
            httpTask->Cancel();
        } else {
            httpTask->SetLen(httpTask->GetLen() + length);
        }
    });
    httpTask->OnCancel([=](const HttpClientRequest& request, const HttpClientResponse& response) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnCancel,cause: %{public}d.", response.GetResponseCode());
        verifyTask->OnPostVerify(uri, response);
        this->HttpSessionTaskStart(verifyTask, urisQueue);
    });
    httpTask->Start();
}

bool AppDomainVerifyTaskMgr::IsIdle()
{
    std::unique_lock<std::shared_mutex> lock(queueMutex_);
    return taskQueue_.empty() && !taskRunning_;
}
}
}
