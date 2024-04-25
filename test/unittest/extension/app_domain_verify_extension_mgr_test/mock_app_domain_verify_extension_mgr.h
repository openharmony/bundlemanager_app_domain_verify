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

#ifndef APP_DOMAIN_VERIFY_MOCK_APP_DOMAIN_VERIFY_EXTENSION_MGR_H
#define APP_DOMAIN_VERIFY_MOCK_APP_DOMAIN_VERIFY_EXTENSION_MGR_H
#include <gmock/gmock.h>
namespace OHOS::AppDomainVerify {
class MockAppDomainVerifyExtensionMgr : public AppDomainVerifyExtensionMgr {
public:
    MOCK_METHOD(bool, Init, (), (override));
    MOCK_METHOD(std::shared_ptr<AppDomainVerifyAgentExt>, GetAppDomainVerifyExt, (const std::string& extName),
        (override));
};
class MockAppDomainVerifyExtensionMgr1 : public AppDomainVerifyExtensionMgr {
public:
    MOCK_METHOD(void*, OpenLib, (), (override));
};
}
#endif  // APP_DOMAIN_VERIFY_MOCK_APP_DOMAIN_VERIFY_EXTENSION_MGR_H
