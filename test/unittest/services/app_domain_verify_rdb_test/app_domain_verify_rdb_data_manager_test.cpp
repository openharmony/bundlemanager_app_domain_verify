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
#include "mock_ffrt/ffrt.h"
#include "mock_constant.h"
#include "rdb_helper.h"
#include "mock_rdb.h"
#define private public
#define protected public
#include "app_domain_verify_rdb_data_manager.h"
#undef private
#undef protected

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
   ASSERT_FALSE(appDomainVerifyRdbDataManager->InsertData("key", "value"));
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

   auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
   ASSERT_FALSE(appDomainVerifyRdbDataManager->InsertData("key", "value"));
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

   auto appDomainVerifyRdbDataManager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
   ASSERT_TRUE(appDomainVerifyRdbDataManager->InsertData("key", "value"));
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
   std::unordered_map<std::string, std::string> dataMap;
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
   std::unordered_map<std::string, std::string> dataMap;
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
   std::unordered_map<std::string, std::string> dataMap;
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
   EXPECT_CALL(*mockResultSet, GoToFirstRow()).Times(1).WillOnce(Return(E_OK));
   EXPECT_CALL(*mockResultSet, GetString(0, _)).Times(1).WillOnce(Return(E_ERROR));
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
   std::unordered_map<std::string, std::string> dataMap;
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
   EXPECT_CALL(*mockResultSet, GoToFirstRow()).Times(1).WillOnce(Return(E_OK));
   EXPECT_CALL(*mockResultSet, GetString(0, _)).Times(1).WillOnce(Return(E_ERROR));
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
   std::unordered_map<std::string, std::string> dataMap;
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
   EXPECT_CALL(*mockResultSet, GoToFirstRow()).Times(1).WillOnce(Return(E_OK));
   EXPECT_CALL(*mockResultSet, GetString(0, _)).Times(1).WillOnce(Return(E_OK));
   EXPECT_CALL(*mockResultSet, GetString(1, _)).Times(1).WillOnce(Return(E_ERROR));
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
   std::unordered_map<std::string, std::string> dataMap;
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
   EXPECT_CALL(*mockResultSet, GoToFirstRow()).Times(1).WillOnce(Return(E_OK));
   EXPECT_CALL(*mockResultSet, GetString(0, _)).Times(1).WillOnce(Return(E_OK));
   EXPECT_CALL(*mockResultSet, GetString(1, _)).Times(1).WillOnce(Return(E_OK));
   EXPECT_CALL(*mockResultSet, GoToNextRow()).Times(1).WillOnce(Return(E_ERROR));
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
   std::unordered_map<std::string, std::string> dataMap;
   ASSERT_TRUE(appDomainVerifyRdbDataManager->QueryAllData(dataMap));
}
}