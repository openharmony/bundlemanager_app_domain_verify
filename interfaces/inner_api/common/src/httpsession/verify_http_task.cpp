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
#include "verify_http_task.h"
namespace OHOS {
namespace AppDomainVerify {
bool VerifyHttpTask::Start()
{
    return HttpClientTask::Start();
}
void VerifyHttpTask::OnSuccess(const std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
        const OHOS::NetStack::HttpClient::HttpClientResponse &response)> &onSucceeded)
{
    HttpClientTask::OnSuccess(onSucceeded);
}
void VerifyHttpTask::OnFail(const std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
        const OHOS::NetStack::HttpClient::HttpClientResponse &response,
        const OHOS::NetStack::HttpClient::HttpClientError &error)> &onFailed)
{
    HttpClientTask::OnFail(onFailed);
}
void VerifyHttpTask::OnDataReceive(
    const std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request, const uint8_t *data,
        size_t length)> &onDataReceive)
{
    HttpClientTask::OnDataReceive(onDataReceive);
}
size_t VerifyHttpTask::GetLen()
{
    return len_;
}
void VerifyHttpTask::SetLen(size_t len)
{
    len_ = len;
}
}
}