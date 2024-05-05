/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef APP_DOMAIN_VERIFY_MOC_VERIFY_TASK_H
#define APP_DOMAIN_VERIFY_MOC_VERIFY_TASK_H
#include <gmock/gmock.h>
#include "i_verify_task.h"
namespace OHOS::AppDomainVerify {
class MocVerifyTask : public IVerifyTask {
public:
    MocVerifyTask(){printf("MocVerifyTask\n");};
    ~MocVerifyTask(){printf("~MocVerifyTask\n");};
    MOCK_METHOD(void, OnPostVerify,
        (const std::string& uri, const OHOS::NetStack::HttpClient::HttpClientResponse& response), (override));
    MOCK_METHOD(void, OnSaveVerifyResult, (), (override));
    MOCK_METHOD(bool, OnPreRequest, (OHOS::NetStack::HttpClient::HttpClientRequest & request, const std::string& uri),
        (override));
    MOCK_METHOD(OHOS::AppDomainVerify::TaskType, GetType, (), (override));
    const std::unordered_map<std::string, InnerVerifyStatus>& GetUriVerifyMap() override
    {
        return map_;
    }
    std::unordered_map<std::string, InnerVerifyStatus> map_;
};

}
#endif  // APP_DOMAIN_VERIFY_MOC_VERIFY_TASK_H
