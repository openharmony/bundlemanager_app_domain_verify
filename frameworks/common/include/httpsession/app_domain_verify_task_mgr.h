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
#include "i_http_task.h"
#include "ffrt.h"
#include "safe_map.h"


namespace OHOS {
namespace AppDomainVerify {
using namespace OHOS::NetStack::HttpClient;
class AppDomainVerifyTaskMgr {
public:
    AppDomainVerifyTaskMgr();
    ~AppDomainVerifyTaskMgr();
    static std::shared_ptr<AppDomainVerifyTaskMgr> GetInstance();
    static void DestroyInstance();
    DISALLOW_COPY_AND_MOVE(AppDomainVerifyTaskMgr);
    bool AddTask(const std::shared_ptr<IHttpTask>& task);
    bool IsIdle();

private:
    static std::shared_ptr<AppDomainVerifyTaskMgr> instance_;
    static std::mutex mutex_;
    bool Init();
    void RunTask(const std::shared_ptr<IHttpTask>& task, uint32_t seq);
    std::shared_ptr<ffrt::queue> ffrtTaskQueue_;
    SafeMap<uint32_t, std::shared_ptr<IHttpTask>> taskHandleMap_;
};
}
}

#endif