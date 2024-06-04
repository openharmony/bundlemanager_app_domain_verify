/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATIONNETSTACK_HTTP_CLIENT_TASK_H
#define COMMUNICATIONNETSTACK_HTTP_CLIENT_TASK_H
#include <memory>
#include <functional>
#include "gmock/gmock.h"
#include "http_client_error.h"
#include "http_client_request.h"
#include "http_client_response.h"

namespace OHOS::NetStack::HttpClient {
#define DATA_BLOCK (10 * 1024)
class HttpClientTask : public std::enable_shared_from_this<HttpClientTask> {
public:
    explicit HttpClientTask(const HttpClientRequest& request)
    {
        GTEST_LOG_(INFO) << "MOCK HttpClientTask";
        request_ = request;
    };
    virtual ~HttpClientTask(){};
    virtual bool Start()
    {
        if (success_) {
            if (onSucceeded_) {
                onSucceeded_(request_, response_);
            }
        }
        if (failed_) {
            if (onFailed_) {
                onFailed_(request_, response_, error_);
            }
        }
        if (receive_) {
            if (onDataReceive_) {
                uint8_t* fake;
                onDataReceive_(request_, fake, DATA_BLOCK);
                onDataReceive_(request_, fake, DATA_BLOCK);
                onDataReceive_(request_, fake, DATA_BLOCK);
            }
        }
        if (cancel_) {
            if (onCanceled_) {
                onCanceled_(request_, response_);
            }
        }
        return true;
    };
    virtual void Cancel()
    {
        GTEST_LOG_(INFO) << "MOCK Cancel";
        if (onCanceled_) {
            onCanceled_(request_, response_);
        }
    };
    virtual void OnSuccess(
        const std::function<void(const HttpClientRequest& request, const HttpClientResponse& response)>& onSucceeded)
    {
        GTEST_LOG_(INFO) << "MOCK OnSuccess";
        onSucceeded_ = onSucceeded;
    };

    virtual void OnCancel(
        const std::function<void(const HttpClientRequest& request, const HttpClientResponse& response)>& onCanceled)
    {
        GTEST_LOG_(INFO) << "MOCK OnCancel";
        onCanceled_ = onCanceled;
    };

    virtual void OnFail(const std::function<void(const HttpClientRequest& request, const HttpClientResponse& response,
            const HttpClientError& error)>& onFailed)
    {
        GTEST_LOG_(INFO) << "MOCK OnFail";
        onFailed_ = onFailed;
    };

    virtual void OnDataReceive(
        const std::function<void(const HttpClientRequest& request, const uint8_t* data, size_t length)>& onDataReceive)
    {
        GTEST_LOG_(INFO) << "MOCK OnDataReceive";
        onDataReceive_ = onDataReceive;
    };
    virtual void SetResponse(const HttpClientResponse& response)
    {
        response_ = response;
    };
    static void MockStatus(bool success = true, bool failed = false, bool receive = false, bool cancel = false)
    {
        success_ = success;
        failed_ = failed;
        receive_ = receive;
        cancel_ = cancel;
    }

    static void MockResponse(HttpClientResponse& response)
    {
        response_ = response;
    }

private:
    inline static bool success_ = true;
    inline static bool failed_ = false;
    inline static bool receive_ = false;
    inline static bool cancel_ = false;
    HttpClientRequest request_;
    inline static HttpClientResponse response_;
    HttpClientError error_;
    std::function<void(const HttpClientRequest& request, const HttpClientResponse& response)> onSucceeded_;
    std::function<void(const HttpClientRequest& request, const HttpClientResponse& response)> onCanceled_;
    std::function<void(
        const HttpClientRequest& request, const HttpClientResponse& response, const HttpClientError& error)>
        onFailed_;
    std::function<void(const HttpClientRequest& request, const uint8_t* data, size_t length)> onDataReceive_;
};
}

#endif  // APP_DOMAIN_VERIFY_HTTP_CLIENT_TASK_H
