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
constexpr int MAX_CONCURRENCY = 10;

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
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "AppDomainVerifyTaskMgr new!");
    Init();
}

AppDomainVerifyTaskMgr::~AppDomainVerifyTaskMgr()
{
    ffrtTaskQueue_.reset();
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "AppDomainVerifyTaskMgr destory!");
}

bool AppDomainVerifyTaskMgr::Init()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "Init.");
    ffrt::queue_attr queueAttr{};
    queueAttr.max_concurrency(MAX_CONCURRENCY);
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
    // hold task's shared_ptr
    taskHandleMap_.EnsureInsert(seq, task);
    auto handle = ffrtTaskQueue_->submit_h([task, seq, this]() { RunTask(task, seq); });
    if (handle == nullptr) {
        AutoSeqReleaser releaser(&taskHandleMap_, seq);
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "submit task to ffrt failed.");
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
        AutoSeqReleaser releaser(&taskHandleMap_, seq);
        return;
    }
    std::weak_ptr<IHttpTask> tmp = task;
    httpclientTask->OnSuccess([tmp, seq, this](const HttpClientRequest& request, const HttpClientResponse& response) {
        AutoSeqReleaser releaser(&taskHandleMap_, seq);
        auto task = tmp.lock();
        if (task == nullptr) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnSuccess can not find task.");
            return;
        }
        task->OnSuccess(request, response);
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnSuccess.");
    });
    httpclientTask->OnFail([tmp, seq, this](const HttpClientRequest& request, const HttpClientResponse& response,
                               const HttpClientError& error) {
        AutoSeqReleaser releaser(&taskHandleMap_, seq);
        auto task = tmp.lock();
        if (task == nullptr) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnFail can not find task.");
            return;
        }
        task->OnFail(request, response, error);
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnFail.");
    });
    httpclientTask->OnCancel([tmp, seq, this](const HttpClientRequest& request, const HttpClientResponse& response) {
        AutoSeqReleaser releaser(&taskHandleMap_, seq);

        auto task = tmp.lock();
        if (task == nullptr) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnCancel can not find task.");
            return;
        }
        task->OnCancel(request, response);
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnCancel.");
    });
    httpclientTask->OnDataReceive(
        [httpclientTask, tmp](const HttpClientRequest& request, const uint8_t* data, size_t length) {
            auto task = tmp.lock();
            if (task == nullptr) {
                APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnDataReceive can not find task.");
                return;
            }
            task->OnDataReceive(httpclientTask, request, data, length);
            APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnDataReceive.");
        });
    httpclientTask->Start();
}
}
}
