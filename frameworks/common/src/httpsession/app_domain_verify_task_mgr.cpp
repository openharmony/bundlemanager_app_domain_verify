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
#include <future>
#include "app_domain_verify_hilog.h"

namespace OHOS {
namespace AppDomainVerify {
std::shared_ptr<AppDomainVerifyTaskMgr> AppDomainVerifyTaskMgr::instance_ = nullptr;
ffrt::mutex AppDomainVerifyTaskMgr::instanceMutex_;

using namespace OHOS::NetStack::HttpClient;

class AutoSeqReleaser {
public:
    AutoSeqReleaser(SafeMap<uint32_t, std::shared_ptr<IHttpTask>>* map, uint32_t seq) : seq_(seq), map_(map)
    {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "AutoSeqReleaser() seq:%{public}u.", seq_);
    }
    ~AutoSeqReleaser()
    {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "~AutoSeqReleaser() seq:%{public}u.", seq_);
        if (map_ != nullptr) {
            map_->Erase(seq_);
        }
    }

private:
    uint32_t seq_;
    SafeMap<uint32_t, std::shared_ptr<IHttpTask>>* map_;
};

std::shared_ptr<AppDomainVerifyTaskMgr> AppDomainVerifyTaskMgr::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<ffrt::mutex> lock(instanceMutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<AppDomainVerifyTaskMgr>();
        }
    }
    return instance_;
}

void AppDomainVerifyTaskMgr::DestroyInstance()
{
    std::lock_guard<ffrt::mutex> lock(instanceMutex_);
    if (instance_ != nullptr) {
        instance_.reset();
        instance_ = nullptr;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "AppDomainVerifyTaskMgr DestroyInstance!");
}

AppDomainVerifyTaskMgr::AppDomainVerifyTaskMgr()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "AppDomainVerifyTaskMgr new!");
    Init();
}

AppDomainVerifyTaskMgr::~AppDomainVerifyTaskMgr()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "AppDomainVerifyTaskMgr destory!");
    {
        std::unique_lock<ffrt::mutex> lock(mutex_);
        this->stop_ = true;
        cond_.notify_all();
    }
    // wait all workers done;
    ffrt::wait();
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "AppDomainVerifyTaskMgr destory end!");
}

bool AppDomainVerifyTaskMgr::Init()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "Init workers.");
    for (int i = 0; i < poolSize_; i++) {
        ffrt::submit_h([this]() {
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "worker run.");
            std::unique_lock<ffrt::mutex> lock(mutex_);
            while (!this->stop_) {
                if (!tasks_.empty()) {
                    auto task = std::move(tasks_.back());
                    tasks_.pop_back();
                    lock.unlock();
                    task();
                    lock.lock();
                } else {
                    while (tasks_.empty() && !this->stop_) {
                        cond_.wait(lock);
                    }
                }
            }
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "worker done.");
        });
    }
    return true;
}
std::function<void()> AppDomainVerifyTaskMgr::GetTaskWrapper(const std::shared_ptr<IHttpTask>& httpTask)
{
    return [taskMap = &taskMap_, httpTask]() {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "task in");
        auto seq = httpTask->GetTaskId();
        auto clientTask = httpTask->CreateHttpClientTask();
        if (clientTask == nullptr) {
            AutoSeqReleaser releaser(taskMap, seq);
            HttpClientRequest request;
            HttpClientResponse response;
            response.SetResult("creat http client task failed");
            HttpClientError error;
            httpTask->OnFail(request, response, error);
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "clientTask null");
            return;
        }
        std::promise<void> promise;
        std::future future = promise.get_future();
        clientTask->OnSuccess(
            [&taskMap, &httpTask, &promise, seq](const HttpClientRequest& request, const HttpClientResponse& response) {
                APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnSuccess");
                httpTask->OnSuccess(request, response);
                AutoSeqReleaser releaser(taskMap, seq);
                promise.set_value();
            });
        clientTask->OnFail([&taskMap, httpTask, &promise, seq](const HttpClientRequest& request,
                               const HttpClientResponse& response, const HttpClientError& error) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE,
                "OnFail, error code:%{public}d, msg:%{public}s", error.GetErrorCode(), error.GetErrorMessage().c_str());
            httpTask->OnFail(request, response, error);
            AutoSeqReleaser releaser(taskMap, seq);
            promise.set_value();
        });
        clientTask->OnCancel(
            [&taskMap, &httpTask, &promise, seq](const HttpClientRequest& request, const HttpClientResponse& response) {
                APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnCancel");
                httpTask->OnCancel(request, response);
                AutoSeqReleaser releaser(taskMap, seq);
                promise.set_value();
            });
        clientTask->OnDataReceive(
            [&clientTask, &httpTask](const HttpClientRequest& request, const uint8_t* data, size_t length) {
                APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnDataReceive");
                httpTask->OnDataReceive(clientTask, request, data, length);
            });
        clientTask->Start();
        future.wait();
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "task out");
    };
}
bool AppDomainVerifyTaskMgr::AddTask(const std::shared_ptr<IHttpTask>& task)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "AddTask.");
    if (task == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AddTask task null.");
        return false;
    }
    std::function<void()> taskWrapper = GetTaskWrapper(task);
    std::unique_lock<ffrt::mutex> lock(mutex_);
    taskMap_.EnsureInsert(task->GetTaskId(), task);
    tasks_.push_back(std::move(taskWrapper));
    cond_.notify_one();
    return true;
}
bool AppDomainVerifyTaskMgr::IsIdle()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "IsIdle.");
    return taskMap_.IsEmpty();
}
}
}
