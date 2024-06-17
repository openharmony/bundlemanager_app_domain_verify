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
#include "i_http_task.h"
#include "app_domain_verify_hilog.h"
namespace OHOS::AppDomainVerify {
static std::atomic<uint32_t> idGen_;
void IHttpTask::OnSuccess(const HttpClientRequest& request, const HttpClientResponse& response)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "%s called", __func__);
}
void IHttpTask::OnCancel(const HttpClientRequest& request, const HttpClientResponse& response)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "%s called", __func__);
}
void IHttpTask::OnFail(
    const HttpClientRequest& request, const HttpClientResponse& response, const HttpClientError& error)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "%s called", __func__);
}
void IHttpTask::OnDataReceive(std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask> task,
    const HttpClientRequest& request, const uint8_t* data, size_t length)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_COMMON, "%s called", __func__);
}
IHttpTask::IHttpTask()
{
    taskId_ = idGen_++;
}
uint32_t IHttpTask::GetTaskId()
{
    return taskId_;
}
}