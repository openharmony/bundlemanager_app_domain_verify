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
#include <memory>
#include "mock_constant.h"
#define private public
#define protected public
#include "bms/bundle_info_query.h"
#undef private
#undef protected
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class BundleInfoQueryTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void BundleInfoQueryTest::SetUpTestCase(void)
{
}

void BundleInfoQueryTest::TearDownTestCase(void)
{
}

void BundleInfoQueryTest::SetUp(void)
{
}

void BundleInfoQueryTest::TearDown(void)
{
}

/**
 * @tc.name: AgentServiceTest001
 * @tc.desc: GetBundleInfo test.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, BundleInfoQueryTest001, TestSize.Level0)
{
    std::string bundleName = BUNDLE_NAME;
    std::string appIdentifier = APP_IDENTIFIER;
    std::string fingerprint = FINGERPRINT;
    ASSERT_TRUE(BundleInfoQuery::GetBundleInfo(bundleName, appIdentifier, fingerprint));
}
}