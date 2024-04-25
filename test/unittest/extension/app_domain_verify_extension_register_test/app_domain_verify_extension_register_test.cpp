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

#include "gtest/gtest.h"
#define private public
#define protected public
#include "app_domain_verify_extension_register.h"
#undef private
#undef protected

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
class AppDomainVerifyAgentExtImpl : public AppDomainVerifyAgentExt {
public:
    AppDomainVerifyAgentExtImpl() = default;
    virtual ~AppDomainVerifyAgentExtImpl() = default;
};
class AppDomainVerifyExtensionRegisterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AppDomainVerifyExtensionRegisterTest::SetUpTestCase(void)
{
}

void AppDomainVerifyExtensionRegisterTest::TearDownTestCase(void)
{
}

void AppDomainVerifyExtensionRegisterTest::SetUp(void)
{
}

void AppDomainVerifyExtensionRegisterTest::TearDown(void)
{
}

/**
 * @tc.name: AppDomainVerifyExtensionRegisterTest001
 * @tc.desc: ExtensionMgr test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyExtensionRegisterTest, AppDomainVerifyExtensionRegisterTest001, TestSize.Level0)
{
    AppDomainVerifyExtensionRegister appDomainVerifyExtensionRegister;
    std::shared_ptr<AppDomainVerifyExtBase> appDomainVerifyExtBase;
    appDomainVerifyExtensionRegister.RegisterAppDomainVerifyExt("test",
        []() -> std::shared_ptr<AppDomainVerifyAgentExt> { return std::make_shared<AppDomainVerifyAgentExtImpl>(); });

    ASSERT_TRUE(appDomainVerifyExtensionRegister.GetAppDomainVerifyExt("test") != nullptr);
}
/**
 * @tc.name: AppDomainVerifyExtensionRegisterTest002
 * @tc.desc: ExtensionMgr test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyExtensionRegisterTest, AppDomainVerifyExtensionRegisterTest002, TestSize.Level0)
{
    AppDomainVerifyExtensionRegister appDomainVerifyExtensionRegister;
    appDomainVerifyExtensionRegister.RegisterAppDomainVerifyExt("test",
        []() -> std::shared_ptr<AppDomainVerifyAgentExt> { return std::make_shared<AppDomainVerifyAgentExtImpl>(); });

    ASSERT_TRUE(appDomainVerifyExtensionRegister.GetAppDomainVerifyExt("test1") == nullptr);
}
/**
 * @tc.name: AppDomainVerifyExtensionRegisterTest003
 * @tc.desc: ExtensionMgr test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyExtensionRegisterTest, AppDomainVerifyExtensionRegisterTest003, TestSize.Level0)
{
    AppDomainVerifyExtensionRegister appDomainVerifyExtensionRegister;

    ASSERT_TRUE(appDomainVerifyExtensionRegister.GetAppDomainVerifyExt("test") == nullptr);
}
}
