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

using namespace OHOS::NetStack::HttpClient;

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
    Init();
}

AppDomainVerifyTaskMgr::~AppDomainVerifyTaskMgr()
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "AppDomainVerifyTaskMgr destory!");
}

bool AppDomainVerifyTaskMgr::Init()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "Init.");
    ffrt::queue_attr queueAttr{};
    queueAttr.max_concurrency(10);
    queueAttr.qos(ffrt_qos_default);
    ffrtTaskQueue_ = std::make_shared<ffrt::queue>("ffrt-queue", queueAttr);
    return false;
}
bool AppDomainVerifyTaskMgr::AddTask(const std::shared_ptr<IHttpTask>& task)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "AddTask.");
    if (ffrtTaskQueue_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AddTask queue null.");
        return false;
    }
    if (task == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AddTask task null.");
        return false;
    }
    auto seq = task->GetTaskId();
    auto handle = ffrt::submit_h([task, seq, this]() { RunTask(task, seq); });
    if (handle != nullptr) {
        auto taskWrap = std::make_shared<ffrt::task_handle>(std::move(handle));
        taskHandleMap_.EnsureInsert(seq, taskWrap);
    } else {
        return false;
    }

    return true;
}
bool AppDomainVerifyTaskMgr::IsIdle()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "IsIdle.");
    return taskHandleMap_.Size() == 0;
}

void AppDomainVerifyTaskMgr::RunTask(const std::shared_ptr<IHttpTask>& task, uint32_t seq)
{
    auto httpclientTask = task->CreateHttpClientTask();
    if (httpclientTask == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "can not get http task.");
        taskHandleMap_.Erase(seq);
        return;
    }
    httpclientTask->OnSuccess([=](const HttpClientRequest& request, const HttpClientResponse& response) {
        task->OnSuccess(request, response);
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnSuccess.");
        taskHandleMap_.Erase(seq);
    });
    httpclientTask->OnFail(
        [=](const HttpClientRequest& request, const HttpClientResponse& response, const HttpClientError& error) {
            task->OnFail(request, response, error);
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnFail.%{public}d.", taskHandleMap_.Size());
            taskHandleMap_.Erase(seq);
        });

    httpclientTask->OnCancel([=](const HttpClientRequest& request, const HttpClientResponse& response) {
        task->OnCancel(request, response);
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnCancel.");
        taskHandleMap_.Erase(seq);
    });
    httpclientTask->OnDataReceive([=](const HttpClientRequest& request, const uint8_t* data, size_t length) {
        task->OnDataReceive(httpclientTask, request, data, length);
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnDataReceive.");
        taskHandleMap_.Erase(seq);
    });
    httpclientTask->Start();
}
}
}
