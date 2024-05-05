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
#include "app_domain_verify_data_mgr.h"
#undef private
#undef protected
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class MgrDataMgrTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void MgrDataMgrTest::SetUpTestCase(void)
{
}

void MgrDataMgrTest::TearDownTestCase(void)
{
}

void MgrDataMgrTest::SetUp(void)
{
}

void MgrDataMgrTest::TearDown(void)
{
}

/**
 * @tc.name: MgrDataMgrTest001
 * @tc.desc: DataMgr test
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrTest001, TestSize.Level0)
{
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    std::string bundleName = "";
    VerifyResultInfo verifyResultInfo;
    ASSERT_FALSE(appDomainVerifyDataMgr->GetVerifyStatus(bundleName, verifyResultInfo));
    ASSERT_FALSE(appDomainVerifyDataMgr->SaveVerifyStatus(bundleName, verifyResultInfo));
    ASSERT_FALSE(appDomainVerifyDataMgr->DeleteVerifyStatus(bundleName));
    appDomainVerifyDataMgr->DeleteVerifyStatus(BUNDLE_NAME);
    ASSERT_TRUE(appDomainVerifyDataMgr->GetAllVerifyStatus().empty());
}
}