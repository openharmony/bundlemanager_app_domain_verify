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
#ifndef APP_DOMAIN_VERIFY_TASK_H
#define APP_DOMAIN_VERIFY_TASK_H

#include <deque>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <unordered_map>
#include "singleton.h"
#include "inner_verify_status.h"
#include "app_verify_base_info.h"
#include "verify_http_task_factory.h"
#include "i_verify_task.h"

namespace OHOS {
namespace AppDomainVerify {

class AppDomainVerifyTaskMgr {
public:
    AppDomainVerifyTaskMgr();
    ~AppDomainVerifyTaskMgr();
    static std::shared_ptr<AppDomainVerifyTaskMgr> GetInstance();
    static void DestroyInstance();
    DISALLOW_COPY_AND_MOVE(AppDomainVerifyTaskMgr);

    bool AddTask(const std::shared_ptr<IVerifyTask> &task);
    bool IsIdle();

private:
    void Run();
    void HttpSessionTaskStart(const std::shared_ptr<IVerifyTask> &verifyTask,
        std::queue<std::vector<std::string>> urisQueue);

private:
    std::deque<std::shared_ptr<IVerifyTask>> taskQueue_;
    std::shared_mutex queueMutex_;
    std::atomic<bool> taskRunning_ = false;
    std::unique_ptr<VerifyHttpTaskFactory> httpClientTaskFactory_;
    static std::shared_ptr<AppDomainVerifyTaskMgr> instance_;
    static std::mutex mutex_;
    void StartHttpTask(const std::shared_ptr<IVerifyTask>& verifyTask, std::queue<std::vector<std::string>>& urisQueue,
        const NetStack::HttpClient::HttpClientRequest& httpClientRequest, const std::string& uri);
};
}
}

#endif