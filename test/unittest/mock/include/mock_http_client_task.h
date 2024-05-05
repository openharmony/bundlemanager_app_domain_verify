/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef MOC_HTTP_CLIENT_TASK_H
#define MOC_HTTP_CLIENT_TASK_H
#include <gmock/gmock.h>
#include "verify_http_task.h"
#include "app_domain_verify_hilog.h"

namespace OHOS::AppDomainVerify {
const int block10k = 10*1024;
class MocHttpClientTask : public VerifyHttpTask {
public:
    explicit MocHttpClientTask(const OHOS::NetStack::HttpClient::HttpClientRequest& request)
        : VerifyHttpTask(request){};

    MocHttpClientTask(const OHOS::NetStack::HttpClient::HttpClientRequest& request,
        OHOS::NetStack::HttpClient::TaskType type, const std::string& filePath)
        : VerifyHttpTask(request, type, filePath){};

    virtual ~MocHttpClientTask(){};

    bool Start()
    {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "%s called", __func__);
        if (sIsCancel) {
            if (onCanceled_) {
                OHOS::NetStack::HttpClient::HttpClientRequest request;
                OHOS::NetStack::HttpClient::HttpClientResponse response;
                response.SetResponseCode(OHOS::NetStack::HttpClient::ResponseCode::OK);
                std::string jsonString =
                    R"({"applinking":{"apps":[{"appIdentifier":"appIdentifier","bundleName":"com.openHarmony.test","fingerprint":"fingerprint"}]}})";
                response.SetResult(jsonString);
                onSucceeded_(request, response);
            }
        }
        if (sIsDataRecv) {
            if (onDataReceive_) {
                OHOS::NetStack::HttpClient::HttpClientRequest request;
                OHOS::NetStack::HttpClient::HttpClientResponse response;
                response.SetResponseCode(OHOS::NetStack::HttpClient::ResponseCode::OK);
                std::string jsonString =
                    R"({"applinking":{"apps":[{"appIdentifier":"appIdentifier","bundleName":"com.openHarmony.test","fingerprint":"fingerprint"}]}})";
                response.SetResult(jsonString);
                onDataReceive_(request, nullptr, block10k);
                onDataReceive_(request, nullptr, block10k);
                onDataReceive_(request, nullptr, block10k);
            }
        }
        if (sTaskRunOk) {
            if (onSucceeded_) {
                OHOS::NetStack::HttpClient::HttpClientRequest request;
                OHOS::NetStack::HttpClient::HttpClientResponse response;
                response.SetResponseCode(OHOS::NetStack::HttpClient::ResponseCode::OK);
                std::string jsonString =
                    R"({"applinking":{"apps":[{"appIdentifier":"appIdentifier","bundleName":"com.openHarmony.test","fingerprint":"fingerprint"}]}})";
                response.SetResult(jsonString);
                onSucceeded_(request, response);
            }
        } else {
            if (sHttpOk) {
                OHOS::NetStack::HttpClient::HttpClientRequest request;
                OHOS::NetStack::HttpClient::HttpClientResponse response;
                OHOS::NetStack::HttpClient::HttpClientError error;
                response.SetResponseCode(OHOS::NetStack::HttpClient::ResponseCode::INTERNAL_ERROR);
                onSucceeded_(request, response);
            } else {
                if (onFailed_) {
                    OHOS::NetStack::HttpClient::HttpClientRequest request;
                    OHOS::NetStack::HttpClient::HttpClientResponse response;
                    OHOS::NetStack::HttpClient::HttpClientError error;
                    response.SetResponseCode(OHOS::NetStack::HttpClient::ResponseCode::INTERNAL_ERROR);
                    onFailed_(request, response, error);
                }
            }
        }
        return true;
    }

    void OnSuccess(const std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
            const OHOS::NetStack::HttpClient::HttpClientResponse &response)> &onSucceeded)
    {
        onSucceeded_ = onSucceeded;
    }

    void OnFail(const std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
            const OHOS::NetStack::HttpClient::HttpClientResponse &response,
            const OHOS::NetStack::HttpClient::HttpClientError &error)> &onFailed)
    {
        onFailed_ = onFailed;
    }
    void OnDataReceive(const std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
            const uint8_t *data, size_t length)> &onDataReceive)
    {
        onDataReceive_ = onDataReceive;
    }

    static bool sIsDataRecv;
    static bool sIsCancel;
    static bool sTaskRunOk;
    static bool sHttpOk;

private:
    std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
        const OHOS::NetStack::HttpClient::HttpClientResponse &response)>
        onSucceeded_;
    std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request,
        const OHOS::NetStack::HttpClient::HttpClientResponse &response,
        const OHOS::NetStack::HttpClient::HttpClientError &error)>
        onFailed_;
    std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest &request, const uint8_t *data,
        size_t length)>
        onDataReceive_;
    std::function<void(const OHOS::NetStack::HttpClient::HttpClientRequest& request,
        const OHOS::NetStack::HttpClient::HttpClientResponse& response)>
        onCanceled_;
};
}

#endif