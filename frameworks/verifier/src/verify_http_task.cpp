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
#include <utility>
#include "verify_http_task.h"
#include "app_domain_verify_hilog.h"

namespace OHOS {
namespace AppDomainVerify {
const static size_t MAX_RESPONSE_LEN = 20 * 1024;  // 20K

VerifyHttpTask::VerifyHttpTask(std::string uri, std::shared_ptr<IVerifyTask> verifyTask)
    : uri_(std::move(uri)), verifyTask_(verifyTask)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "VerifyHttpTask.");
}
std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask> VerifyHttpTask::CreateHttpClientTask()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "CreateHttpClientTask.");
    OHOS::NetStack::HttpClient::HttpClientRequest httpClientRequest;
    if (verifyTask_) {
        if (!verifyTask_->OnPreRequest(httpClientRequest, uri_)) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnPreRequest failed.");
            return nullptr;
        }
        auto httpClientTask = std::make_shared<OHOS::NetStack::HttpClient::HttpClientTask>(httpClientRequest);
        return httpClientTask;
    }
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "CreateHttpClientTask failed.");
    return nullptr;
}
void VerifyHttpTask::OnSuccess(const HttpClientRequest& request, const HttpClientResponse& response)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnSuccess.");
    if (verifyTask_) {
        verifyTask_->OnPostVerify(uri_, response);
    }
}
void VerifyHttpTask::OnFail(
    const HttpClientRequest& request, const HttpClientResponse& response, const HttpClientError& error)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnFail.");
    if (verifyTask_) {
        verifyTask_->OnPostVerify(uri_, response);
    }
}
void VerifyHttpTask::OnCancel(const HttpClientRequest& request, const HttpClientResponse& response)
{
    APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnCance.");
    if (verifyTask_) {
        verifyTask_->OnPostVerify(uri_, response);
    }
}
void VerifyHttpTask::OnDataReceive(std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask> task,
    const HttpClientRequest& request, const uint8_t* data, size_t length)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "OnDataReceive size:%{public}zu.", length);
    if (length_ + length > MAX_RESPONSE_LEN) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "recieve data exceeds, will cancel.");
        task->Cancel();
    } else {
        length_ += length;
    }
}
VerifyHttpTask::~VerifyHttpTask()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "~VerifyHttpTask.");
}
}
}