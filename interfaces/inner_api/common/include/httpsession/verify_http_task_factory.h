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
#ifndef APP_DOMAIN_HTTP_CLIENT_TASK_FACTORY_H
#define APP_DOMAIN_HTTP_CLIENT_TASK_FACTORY_H

#include "http_client.h"
#include "verify_http_task.h"

namespace OHOS {
namespace AppDomainVerify {

class VerifyHttpTaskFactory {
public:
    VerifyHttpTaskFactory() = default;
    virtual ~VerifyHttpTaskFactory() = default;
    virtual std::shared_ptr<VerifyHttpTask> CreateTask(
        const OHOS::NetStack::HttpClient::HttpClientRequest &httpClientRequest);
};
}
}
#endif
