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
#include "app_domain_verify_rdb_data_manager.h"
#undef private
#undef protected
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
class RdbDataMgrTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void RdbDataMgrTest::SetUpTestCase(void)
{
}

void RdbDataMgrTest::TearDownTestCase(void)
{
}

void RdbDataMgrTest::SetUp(void)
{
}

void RdbDataMgrTest::TearDown(void)
{
}

/**
 * @tc.name: RdbDataMgrTest001
 * @tc.desc: Check test
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrTest001, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    ASSERT_FALSE(appDomainVerifyRdbDataManager->CheckRdbReturnIfOk(NativeRdb::E_DB_NOT_EXIST));
    ASSERT_FALSE(appDomainVerifyRdbDataManager->CheckRdbStoreExist(nullptr));
}
}