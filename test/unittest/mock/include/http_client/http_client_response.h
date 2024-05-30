/*
* Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATIONNETSTACK_HTTP_CLIENT_RESPONSE_H
#define COMMUNICATIONNETSTACK_HTTP_CLIENT_RESPONSE_H
#include <string>
#include <gmock/gmock.h>
namespace OHOS {
namespace NetStack {
namespace HttpClient {
using ResponseCode = int;
class HttpClientResponse {
public:
    HttpClientResponse() : responseCode_(0) {};
    ResponseCode GetResponseCode() const{
        GTEST_LOG_(INFO) << "MOCK GetResponseCode";
        return responseCode_;
    }
    const std::string &GetResult() const{
        GTEST_LOG_(INFO) << "MOCK GetResult";
        return result_;
    }
    int responseCode_ = 0;
    std::string result_;
};
}
}
}
#endif  // COMMUNICATIONNETSTACK_HTTP_CLIENT_RESPONSE_H
