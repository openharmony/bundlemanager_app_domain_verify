/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef APP_DOMAIN_VERIFY_HTTP_TASK_H
#define APP_DOMAIN_VERIFY_HTTP_TASK_H
#include "i_http_task.h"
#include "i_verify_task.h"
namespace OHOS::AppDomainVerify {
class VerifyHttpTask : public IHttpTask {
public:
    VerifyHttpTask(std::string uri, std::shared_ptr<IVerifyTask> verifyTask);
    ~VerifyHttpTask() override;
    std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask> CreateHttpClientTask() override;
    void OnSuccess(const HttpClientRequest& request, const HttpClientResponse& response) override;

    void OnCancel(const HttpClientRequest& request, const HttpClientResponse& response) override;

    void OnFail(
        const HttpClientRequest& request, const HttpClientResponse& response, const HttpClientError& error) override;

    void OnDataReceive(std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask> task,
        const HttpClientRequest& request, const uint8_t* data, size_t length) override;

private:
    std::string uri_;
    std::shared_ptr<IVerifyTask> verifyTask_;
    size_t length_{ 0 };
};
}

#endif  // APP_DOMAIN_VERIFY_HTTP_TASK_H
