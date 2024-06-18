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

#ifndef APP_DOMAIN_VERIFY_I_HTTP_TASK_H
#define APP_DOMAIN_VERIFY_I_HTTP_TASK_H
#include "http_client_response.h"
#include "http_client_request.h"
#include "http_client_task.h"
#include "http_client.h"
namespace OHOS::AppDomainVerify {
using namespace OHOS::NetStack::HttpClient;
class IHttpTask {
public:
    IHttpTask();
    virtual ~IHttpTask() = default;
    virtual std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask> CreateHttpClientTask() = 0;
    virtual void OnSuccess(const HttpClientRequest& request, const HttpClientResponse& response) = 0;

    virtual void OnCancel(const HttpClientRequest& request, const HttpClientResponse& response) = 0;

    virtual void OnFail(
        const HttpClientRequest& request, const HttpClientResponse& response, const HttpClientError& error) = 0;

    virtual void OnDataReceive(std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask> task,
        const HttpClientRequest& request, const uint8_t* data, size_t length) = 0;
    uint32_t GetTaskId();

private:
    uint32_t taskId_;
};
}
#endif  // APP_DOMAIN_VERIFY_I_HTTP_TASK_H
