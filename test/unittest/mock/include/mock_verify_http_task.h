/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef APP_DOMAIN_VERIFY_MOCK_VERIFY_HTTP_TASK_H
#define APP_DOMAIN_VERIFY_MOCK_VERIFY_HTTP_TASK_H
#include "gmock/gmock.h"
#include "i_http_task.h"
namespace OHOS {
namespace AppDomainVerify {
class MocVerifyHttpTask : public IHttpTask {
public:
    MOCK_METHOD(std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask>, CreateHttpClientTask, (), (override));
    MOCK_METHOD(void, OnSuccess, (const HttpClientRequest& request, const HttpClientResponse& response), (override));

    MOCK_METHOD(void, OnCancel, (const HttpClientRequest& request, const HttpClientResponse& response), (override));

    MOCK_METHOD(void, OnFail,
        (const HttpClientRequest& request, const HttpClientResponse& response, const HttpClientError& error),
        (override));

    MOCK_METHOD(void, OnDataReceive,
        (std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask> task, const HttpClientRequest& request,
            const uint8_t* data, size_t length),
        (override));
};
}
}
#endif  // APP_DOMAIN_VERIFY_MOCK_VERIFY_HTTP_TASK_H
