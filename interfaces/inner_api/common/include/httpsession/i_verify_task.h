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

#ifndef APP_DOMAIN_VERIFY_I_VERIFY_TASK_H
#define APP_DOMAIN_VERIFY_I_VERIFY_TASK_H
#include "http_client_response.h"
#include "http_client.h"
#include "app_domain_verify_hisysevent.h"
#include "inner_verify_status.h"

namespace OHOS {
namespace AppDomainVerify {
class IVerifyTask {
public:
    IVerifyTask() = default;
    virtual ~IVerifyTask() = default;
    virtual void OnPostVerify(const std::string &uri,
        const OHOS::NetStack::HttpClient::HttpClientResponse &response) = 0;
    virtual void OnSaveVerifyResult() = 0;
    virtual bool OnPreRequest(OHOS::NetStack::HttpClient::HttpClientRequest &request, const std::string &uri) = 0;
    virtual const std::unordered_map<std::string, InnerVerifyStatus> &GetUriVerifyMap() = 0;
    virtual OHOS::AppDomainVerify::TaskType GetType() = 0;
};
}
}
#endif  // APP_DOMAIN_VERIFY_I_VERIFY_TASK_H
