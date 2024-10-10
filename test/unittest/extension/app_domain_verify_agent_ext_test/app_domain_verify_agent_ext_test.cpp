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
#include "app_domain_verify_agent_ext.h"
#undef private
#undef protected
#include "app_domain_verify_hilog.h"
#include "app_domain_verify_agent_ext.h"
#include "mock_constant.h"
#include "mock_convert_callback.h"
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
class AppDomainVerifyAgentExtImpl : public AppDomainVerifyAgentExt {
public:
    AppDomainVerifyAgentExtImpl() = default;
    virtual ~AppDomainVerifyAgentExtImpl() = default;
};

class AppDomainVerifyAgentExtTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void AppDomainVerifyAgentExtTest::SetUpTestCase(void)
{
}

void AppDomainVerifyAgentExtTest::TearDownTestCase(void)
{
}

void AppDomainVerifyAgentExtTest::SetUp(void)
{
}

void AppDomainVerifyAgentExtTest::TearDown(void)
{
}

/**
 * @tc.name: AppDomainVerifyAgentExtTest001
 * @tc.desc: ExtensionMgr test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentExtTest, AppDomainVerifyAgentExtTest001, TestSize.Level0)
{
    AppDomainVerifyAgentExt appDomainVerifyAgentExt;
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfo.fingerprint = FINGERPRINT;
    VerifyResultInfo verifyResultInfo;
    ASSERT_TRUE(appDomainVerifyAgentExt.SingleVerify(appVerifyBaseInfo, verifyResultInfo) ==
        ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND);
}
/**
 * @tc.name: AppDomainVerifyAgentExtTest002
 * @tc.desc: ExtensionMgr test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentExtTest, AppDomainVerifyAgentExtTest002, TestSize.Level0)
{
    AppDomainVerifyAgentExt appDomainVerifyAgentExt;
    const BundleVerifyStatusInfo bundleVerifyStatusInfo;
    int delaySeconds = 0;
    TaskType type = IMMEDIATE_TASK;
    ASSERT_TRUE(appDomainVerifyAgentExt.CompleteVerifyRefresh(bundleVerifyStatusInfo, delaySeconds,
                    type) == ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND);
}
/**
 * @tc.name: AppDomainVerifyAgentExtTest003
 * @tc.desc: ExtensionMgr test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentExtTest, AppDomainVerifyAgentExtTest003, TestSize.Level0)
{
    AppDomainVerifyAgentExt appDomainVerifyAgentExt;
    ASSERT_TRUE(appDomainVerifyAgentExt.UpdateWhiteList() == ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND);
}
/**
 * @tc.name: AppDomainVerifyAgentExtTest004
 * @tc.desc: ExtensionMgr test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentExtTest, AppDomainVerifyAgentExtTest004, TestSize.Level0)
{
    AppDomainVerifyAgentExt appDomainVerifyAgentExt;
    OHOS::AAFwk::Want atomicWant;
    sptr<IConvertCallback> cb = new MocConvertCallback;

    ASSERT_TRUE(appDomainVerifyAgentExt.ConvertToExplicitWant(atomicWant, cb) == ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND);
}
}
