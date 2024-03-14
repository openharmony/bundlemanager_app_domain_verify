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

#include <gtest/gtest.h>
#define private public
#define protected public
#include "app_domain_verify_extension_register.h"
#undef private
#undef protected

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class AppDomainVerifyExtTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AppDomainVerifyExtTest::SetUpTestCase(void)
{
}

void AppDomainVerifyExtTest::TearDownTestCase(void)
{
}

void AppDomainVerifyExtTest::SetUp(void)
{
}

void AppDomainVerifyExtTest::TearDown(void)
{
}

/**
 * @tc.name: AppDomainVerifyExtTest001
 * @tc.desc: ext test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyExtTest, AppDomainVerifyExtTest001, TestSize.Level0)
{
    std::string extName = "ext";
    AppDomainVerifyExtensionRegister::GetInstance().RegisterAppDomainVerifyExt(extName,
        []() -> std::shared_ptr<AppDomainVerifyExtBase> { return std::make_shared<AppDomainVerifyExtBase>(); });
    ASSERT_TRUE(AppDomainVerifyExtensionRegister::GetInstance().GetAppDomainVerifyExt(extName) != nullptr);
}
}
