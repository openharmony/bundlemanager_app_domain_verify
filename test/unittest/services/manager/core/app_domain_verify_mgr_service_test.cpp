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
#include <memory>
#include "mock_constant.h"
#include "rdb_helper.h"
#define private public
#define protected public
#include "app_domain_verify_mgr_service.h"
#undef private
#undef protected
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class MgrServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void MgrServiceTest::SetUpTestCase(void)
{
}

void MgrServiceTest::TearDownTestCase(void)
{
}

void MgrServiceTest::SetUp(void)
{
}

void MgrServiceTest::TearDown(void)
{
}

/**
 * @tc.name: MgrServiceTest001
 * @tc.desc: ClearDomainVerifyStatus test
 * @tc.type: FUNC
*/
HWTEST_F(MgrServiceTest, MgrServiceTest001, TestSize.Level0)
{
    auto appDomainVerifyMgrService = std::make_shared<AppDomainVerifyMgrService>();
    appDomainVerifyMgrService->OnStart();
    ASSERT_TRUE(appDomainVerifyMgrService->ClearDomainVerifyStatus(APP_IDENTIFIER, BUNDLE_NAME));
    appDomainVerifyMgrService->OnStop();
}
}