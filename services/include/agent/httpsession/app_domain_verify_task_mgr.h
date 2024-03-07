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
#include <map>
#include "singleton.h"
#include "inner_verify_status.h"
#include "app_verify_base_info.h"
#include "verify_http_task_factory.h"

namespace OHOS {
namespace AppDomainVerify {
typedef enum EnumTaskType {
    IMMEDIATE_TASK,
    FULL_REFRESH_TASK,
    UNKNOWN_TASK
} TaskType;

class Task {
public:
    Task(int type, const AppVerifyBaseInfo &appVerifyBaseInfo,
        const std::unordered_map<std::string, InnerVerifyStatus> &uriVerifyMap)
        : type_(type),
          appVerifyBaseInfo_(appVerifyBaseInfo),
          uriVerifyMap_(uriVerifyMap){};
    virtual ~Task(){};
    int type_;
    AppVerifyBaseInfo appVerifyBaseInfo_;
    std::unordered_map<std::string, InnerVerifyStatus> uriVerifyMap_;
};

class AppDomainVerifyTaskMgr {
public:
    API_EXPORT AppDomainVerifyTaskMgr();
    API_EXPORT virtual ~AppDomainVerifyTaskMgr() = default;

    bool AddTask(const std::shared_ptr<Task> &task);
    bool IsIdle();

private:
    void Run();
    void HttpSessionTaskStart(const std::shared_ptr<Task> &verifyTask, std::queue<std::vector<std::string>> urisQueue);
    void SaveVerifyResult(const AppVerifyBaseInfo &appVerifyBaseInfo,
        const std::unordered_map<std::string, InnerVerifyStatus> &uriVerifyMap);

private:
    std::deque<std::shared_ptr<Task>> taskQueue;
    std::shared_mutex queueMutex;
    std::atomic<bool> taskRunning = false;
    std::unique_ptr<VerifyHttpTaskFactory> httpClientTaskFactory_;
};
}
}

#endif