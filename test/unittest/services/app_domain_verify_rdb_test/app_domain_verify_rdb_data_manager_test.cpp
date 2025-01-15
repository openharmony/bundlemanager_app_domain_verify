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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <memory>
#include "mock_ffrt/ffrt.h"
#include "mock_constant.h"
#include "rdb_helper.h"
#include "mock_rdb.h"
#define private public
#define protected public
#include "app_domain_verify_rdb_data_manager.h"
#undef private
#undef protected
#include "rdb_migrate_mgr.h"
#include "mock_rdb.h"
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
using namespace OHOS::NativeRdb;
class RdbDataMgrTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void RdbDataMgrTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase";
}

void RdbDataMgrTest::TearDownTestCase(void)
{
    MockGetRdbStore(nullptr);
    GTEST_LOG_(INFO) << "TearDownTestCase";
}

void RdbDataMgrTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "SetUp";
}

void RdbDataMgrTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "TearDown";
}

/**
 * @tc.name: RdbDataMgrCreateTableTest001
 * @tc.desc: Callback test
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrCreateTableTest001, TestSize.Level0)
{
    MockGetRdbStore(nullptr);
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    ASSERT_FALSE(appDomainVerifyRdbDataManager->CreateTable());
}
/**
 * @tc.name: RdbDataMgrCreateTableTest002
 * @tc.desc: Create table, rdbstore executesql failed
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrCreateTableTest002, TestSize.Level0)
{
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, ExecuteSql(_, _)).Times(1).WillOnce(Return(E_ERROR));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    ASSERT_FALSE(appDomainVerifyRdbDataManager->CreateTable());
}
/**
 * @tc.name: RdbDataMgrCreateTableTest003
 * @tc.desc: Create table, rdbstore executesql ok
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrCreateTableTest003, TestSize.Level0)
{
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, ExecuteSql(_, _)).Times(1).WillOnce(Return(E_OK));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    ASSERT_TRUE(appDomainVerifyRdbDataManager->CreateTable());
}
/**
 * @tc.name: RdbDataMgrInsertTest001
 * @tc.desc: Insert, rdbstore null
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrInsertTest001, TestSize.Level0)
{
    MockGetRdbStore(nullptr);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    RdbDataItem item;
    ASSERT_FALSE(appDomainVerifyRdbDataManager->InsertData(item));
}
/**
 * @tc.name: RdbDataMgrInsertTest002
 * @tc.desc: Insert, insert fail
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrInsertTest002, TestSize.Level0)
{
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, InsertWithConflictResolution(_, _, _, _)).Times(1).WillOnce(Return(-1));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");
    RdbDataItem item;
    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    ASSERT_FALSE(appDomainVerifyRdbDataManager->InsertData(item));
}
/**
 * @tc.name: RdbDataMgrInsertTest003
 * @tc.desc: Insert, insert ok
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrInsertTest003, TestSize.Level0)
{
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, InsertWithConflictResolution(_, _, _, _)).Times(1).WillOnce(Return(0));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");
    RdbDataItem item;
    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    ASSERT_TRUE(appDomainVerifyRdbDataManager->InsertData(item));
}
/**
 * @tc.name: RdbDataMgrDeleteDataTest001
 * @tc.desc: Delete data, rdbstore null
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrDeleteDataTest001, TestSize.Level0)
{
    MockGetRdbStore(nullptr);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    ASSERT_FALSE(appDomainVerifyRdbDataManager->DeleteData("key"));
}
/**
 * @tc.name: RdbDataMgrDeleteDataTest002
 * @tc.desc: Delete data, delete fail
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrDeleteDataTest002, TestSize.Level0)
{
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, Delete(_, _)).Times(1).WillOnce(Return(-1));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    ASSERT_FALSE(appDomainVerifyRdbDataManager->DeleteData("key"));
}
/**
 * @tc.name: RdbDataMgrDeleteDataTest003
 * @tc.desc: Delete data, delete ok
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrDeleteDataTest003, TestSize.Level0)
{
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, Delete(_, _)).Times(1).WillOnce(Return(0));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    ASSERT_TRUE(appDomainVerifyRdbDataManager->DeleteData("key"));
}
/**
 * @tc.name: RdbDataMgrQueryDataTest001
 * @tc.desc: Query data, rdbstore null
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrQueryDataTest001, TestSize.Level0)
{
    MockGetRdbStore(nullptr);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    std::unordered_map<std::string, std::vector<RdbDataItem>> dataMap;
    ASSERT_FALSE(appDomainVerifyRdbDataManager->QueryAllData(dataMap));
}
/**
 * @tc.name: RdbDataMgrQueryDataTest002
 * @tc.desc: Query data, Query null
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrQueryDataTest002, TestSize.Level0)
{
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, Query(_, _)).Times(1).WillOnce(Return(nullptr));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    std::unordered_map<std::string, std::vector<RdbDataItem>> dataMap;
    ASSERT_FALSE(appDomainVerifyRdbDataManager->QueryAllData(dataMap));
}
/**
 * @tc.name: RdbDataMgrQueryDataTest003
 * @tc.desc: Query data, result getfirstrow failed
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrQueryDataTest003, TestSize.Level0)
{
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).Times(1).WillOnce(Return(E_ERROR));
    EXPECT_CALL(*mockResultSet, Close()).Times(1).WillOnce(Return(E_OK));
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, Query(_, _)).Times(1).WillOnce(Return(mockResultSet));
    MockGetRdbStore(mocRdbStore);
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    std::unordered_map<std::string, std::vector<RdbDataItem>> dataMap;
    ASSERT_TRUE(appDomainVerifyRdbDataManager->QueryAllData(dataMap));
}
/**
 * @tc.name: RdbDataMgrQueryDataTest004
 * @tc.desc: Query data, result getstring failed
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrQueryDataTest004, TestSize.Level0)
{
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).Times(AtLeast(1)).WillRepeatedly(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetString(1, _)).Times(AtLeast(1)).WillRepeatedly(Return(E_ERROR));
    EXPECT_CALL(*mockResultSet, Close()).Times(1).WillOnce(Return(E_OK));
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, Query(_, _)).Times(1).WillOnce(Return(mockResultSet));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    std::unordered_map<std::string, std::vector<RdbDataItem>> dataMap;
    ASSERT_FALSE(appDomainVerifyRdbDataManager->QueryAllData(dataMap));
}
/**
 * @tc.name: RdbDataMgrQueryDataTest005
 * @tc.desc: Query data, result getstring failed
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrQueryDataTest005, TestSize.Level0)
{
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).Times(AtLeast(1)).WillRepeatedly(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetString(1, _)).Times(AtLeast(1)).WillRepeatedly(Return(E_ERROR));
    EXPECT_CALL(*mockResultSet, Close()).Times(1).WillOnce(Return(E_OK));
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, Query(_, _)).Times(1).WillOnce(Return(mockResultSet));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    std::unordered_map<std::string, std::vector<RdbDataItem>> dataMap;
    ASSERT_FALSE(appDomainVerifyRdbDataManager->QueryAllData(dataMap));
}
/**
 * @tc.name: RdbDataMgrQueryDataTest006
 * @tc.desc: Query data, result getstring failed
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrQueryDataTest006, TestSize.Level0)
{
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).Times(AtLeast(1)).WillRepeatedly(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetString(1, _)).Times(AtLeast(1)).WillRepeatedly(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetString(2, _)).Times(AtLeast(1)).WillRepeatedly(Return(E_ERROR));
    EXPECT_CALL(*mockResultSet, Close()).Times(1).WillOnce(Return(E_OK));
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, Query(_, _)).Times(1).WillOnce(Return(mockResultSet));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    std::unordered_map<std::string, std::vector<RdbDataItem>> dataMap;
    ASSERT_FALSE(appDomainVerifyRdbDataManager->QueryAllData(dataMap));
}

/**
 * @tc.name: RdbDataMgrQueryDataTest007
 * @tc.desc: Query data, result go to next failed
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbDataMgrQueryDataTest007, TestSize.Level0)
{
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).Times(AtLeast(1)).WillRepeatedly(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetString(_, _)).Times(AtLeast(1)).WillRepeatedly(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetInt(_, _)).Times(AtLeast(1)).WillRepeatedly(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GoToNextRow()).Times(AtLeast(1)).WillRepeatedly(Return(E_ERROR));
    EXPECT_CALL(*mockResultSet, Close()).Times(1).WillOnce(Return(E_OK));
    std::shared_ptr<MockRdbStore> mocRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mocRdbStore, Query(_, _)).Times(1).WillOnce(Return(mockResultSet));
    MockGetRdbStore(mocRdbStore);

    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    rdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS " + rdbConfig.tableName + "(KEY TEXT NOT NULL PRIMARY KEY, VALUE TEXT NOT NULL);");

    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    std::unordered_map<std::string, std::vector<RdbDataItem>> dataMap;
    ASSERT_TRUE(appDomainVerifyRdbDataManager->QueryAllData(dataMap));
}

/**
 * @tc.name: CheckRdbStoreExist_0100
 * @tc.desc: Test CheckRdbStoreExist.
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, CheckRdbStoreExist_0100, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    std::shared_ptr<NativeRdb::RdbStore> rdbStore = nullptr;
    auto res = appDomainVerifyRdbDataManager->CheckRdbStoreExist(rdbStore);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: CheckRdbStoreExist_0200
 * @tc.desc: Test CheckRdbStoreExist.
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, CheckRdbStoreExist_0200, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    std::shared_ptr<NativeRdb::RdbStore> rdbStore = appDomainVerifyRdbDataManager->GetRdbStore();
    auto res = appDomainVerifyRdbDataManager->CheckRdbStoreExist(rdbStore);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: CheckRdbReturnIfOk_0100
 * @tc.desc: Test CheckRdbReturnIfOk.
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, CheckRdbReturnIfOk_0100, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    int errcode = NativeRdb::E_OK;
    auto res = appDomainVerifyRdbDataManager->CheckRdbReturnIfOk(errcode);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: CheckRdbReturnIfOk_0200
 * @tc.desc: Test CheckRdbReturnIfOk.
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, CheckRdbReturnIfOk_0200, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    int errcode = NativeRdb::E_ERROR;
    auto res = appDomainVerifyRdbDataManager->CheckRdbReturnIfOk(errcode);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: GetRdbStore_0100
 * @tc.desc: Test GetRdbStore.
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, GetRdbStore_0100, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    auto res = appDomainVerifyRdbDataManager->GetRdbStore();
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name: DelayCloseRdbStore_0100
 * @tc.desc: Test DelayCloseRdbStore.
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, DelayCloseRdbStore_0100, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    appDomainVerifyRdbDataManager->continuationHandler_ = nullptr;
    appDomainVerifyRdbDataManager->DelayCloseRdbStore();
    EXPECT_NE(appDomainVerifyRdbDataManager->continuationHandler_, nullptr);
}

static int g_innerCnt = -1;
int GetColumnCount(int& count)
{
    count = g_innerCnt;
    return 0;
}
/**
 * @tc.name: RdbMigrateTest001
 * @tc.desc: Upgrade 0
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbMigrateTest001, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    RdbMigrateMgr rdbMigrateMgr(rdbConfig);

    MockRdbStore mocRdbStore;
    ASSERT_TRUE(rdbMigrateMgr.Upgrade(mocRdbStore, 0, 0) == NativeRdb::E_OK);
}
/**
 * @tc.name: RdbMigrateTest002
 * @tc.desc: Upgrade 1 to 2 to 3
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbMigrateTest002, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    RdbMigrateMgr rdbMigrateMgr(rdbConfig);

    std::shared_ptr<MockResultSet> absSharedResultSet = std::make_shared<MockResultSet>();
    g_innerCnt = 2;
    EXPECT_CALL(*absSharedResultSet, GetColumnCount(_)).WillOnce(GetColumnCount);
    MockRdbStore mocRdbStore;
    EXPECT_CALL(mocRdbStore, Query(_, _)).Times(1).WillOnce(Return(absSharedResultSet));
    EXPECT_CALL(mocRdbStore, ExecuteSql(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore, BeginTransaction()).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore, Commit()).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore, RollBack()).WillRepeatedly(Return(0));
    ASSERT_TRUE(rdbMigrateMgr.Upgrade(mocRdbStore, 1, 0) == NativeRdb::E_OK);
}
/**
 * @tc.name: RdbMigrateTest003
 * @tc.desc: Upgrade 2 to 3
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbMigrateTest003, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    RdbMigrateMgr rdbMigrateMgr(rdbConfig);

    MockRdbStore mocRdbStore;
    EXPECT_CALL(mocRdbStore, ExecuteSql(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore, BeginTransaction()).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore, Commit()).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore, RollBack()).WillRepeatedly(Return(0));
    ASSERT_TRUE(rdbMigrateMgr.Upgrade(mocRdbStore, 2, 0) == NativeRdb::E_OK);
}
/**
 * @tc.name: RdbMigrateTest004
 * @tc.desc: Upgrade 3 to other
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbMigrateTest004, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    RdbMigrateMgr rdbMigrateMgr(rdbConfig);

    MockRdbStore mocRdbStore;
    ASSERT_TRUE(rdbMigrateMgr.Upgrade(mocRdbStore, 3, 0) == NativeRdb::E_OK);
}
/**
 * @tc.name: RdbMigrateTest005
 * @tc.desc: Upgrade 2 to 3 abnormal
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbMigrateTest005, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    RdbMigrateMgr rdbMigrateMgr(rdbConfig);

    // begin transaction failed
    MockRdbStore mocRdbStore;
    EXPECT_CALL(mocRdbStore, BeginTransaction()).WillRepeatedly(Return(1));
    ASSERT_FALSE(rdbMigrateMgr.Upgrade(mocRdbStore, 2, 0) == NativeRdb::E_OK);

    // commit failed
    MockRdbStore mocRdbStore2;
    EXPECT_CALL(mocRdbStore2, BeginTransaction()).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore2, ExecuteSql(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore2, Commit()).WillRepeatedly(Return(1));
    ASSERT_FALSE(rdbMigrateMgr.Upgrade(mocRdbStore2, 2, 0) == NativeRdb::E_OK);

    // roll back failed
    MockRdbStore mocRdbStore3;
    EXPECT_CALL(mocRdbStore3, BeginTransaction()).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore3, ExecuteSql(_, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(mocRdbStore3, RollBack()).WillRepeatedly(Return(1));
    ASSERT_FALSE(rdbMigrateMgr.Upgrade(mocRdbStore3, 2, 0) == NativeRdb::E_OK);
}

/**
 * @tc.name: RdbMigrateTest006
 * @tc.desc: Upgrade 1 to 2 abnormal
 * @tc.type: FUNC
 */
HWTEST_F(RdbDataMgrTest, RdbMigrateTest006, TestSize.Level0)
{
    AppDomainVerifyRdbConfig rdbConfig;
    rdbConfig.dbName = Constants::RDB_NAME;
    rdbConfig.tableName = Constants::RDB_TABLE_NAME;
    RdbMigrateMgr rdbMigrateMgr(rdbConfig);

    std::shared_ptr<MockResultSet> absSharedResultSet = std::make_shared<MockResultSet>();
    g_innerCnt = 2;
    EXPECT_CALL(*absSharedResultSet, GetColumnCount(_)).WillOnce(GetColumnCount);
    MockRdbStore mocRdbStore;
    EXPECT_CALL(mocRdbStore, Query(_, _)).Times(1).WillOnce(Return(absSharedResultSet));
    EXPECT_CALL(mocRdbStore, ExecuteSql(_, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(mocRdbStore, BeginTransaction()).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore, Commit()).WillRepeatedly(Return(0));
    EXPECT_CALL(mocRdbStore, RollBack()).WillRepeatedly(Return(0));
    ASSERT_FALSE(rdbMigrateMgr.Upgrade(mocRdbStore, 1, 0) == NativeRdb::E_OK);
}
}