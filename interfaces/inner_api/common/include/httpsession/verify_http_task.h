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
#ifndef APP_DOMAIN_HTTP_CLIENT_TASK_H
#define APP_DOMAIN_HTTP_CLIENT_TASK_H

#include "http_client.h"
namespace OHOS {
namespace AppDomainVerify {

class VerifyHttpTask : public OHOS::NetStack::HttpClient::HttpClientTask {
public:
    VerifyHttpTask(const OHOS::NetStack::HttpClient::HttpClientRequest &request)
        : OHOS::NetStack::HttpClient::HttpClientTask(request){};

    VerifyHttpTask(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
        OHOS::NetStack::HttpClient::TaskType type, const std::string &filePath)
        : OHOS::NetStack::HttpClient::HttpClientTask(request, type, filePath){};

    virtual ~VerifyHttpTask(){};

    virtual bool Start();
    virtual void OnSuccess(const std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
            const OHOS::NetStack::HttpClient::HttpClientResponse &response)> &onSucceeded);
    virtual void OnFail(const std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
            const OHOS::NetStack::HttpClient::HttpClientResponse &response,
            const OHOS::NetStack::HttpClient::HttpClientError &error)> &onFailed);
    virtual void OnDataReceive(const std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
            const uint8_t *data, size_t length)> &onDataReceive);

    size_t GetLen();
    void SetLen(size_t len);
private:
    size_t len_{0};
};
}
}
#endif
