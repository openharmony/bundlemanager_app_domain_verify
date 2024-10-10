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
#include "rdb_helper.h"
#define private public
#define protected public
#include "app_domain_verify_data_mgr.h"
#undef private
#undef protected
#include "mock_rdb_data_manager.h"
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
    MOC::impl = nullptr;
}

void MgrDataMgrTest::SetUp(void)
{
}

void MgrDataMgrTest::TearDown(void)
{
}

bool InvokeQueryAllDataWrongData(std::unordered_map<std::string, std::vector<RdbDataItem>>& dataMap)
{
    std::vector<RdbDataItem> item;
    dataMap.emplace("a", item);
    return false;
}

bool InvokeQueryAllData(std::unordered_map<std::string, std::vector<RdbDataItem>>& dataMap)
{
    std::vector<RdbDataItem> items;
    RdbDataItem item1 = { .bundleName = "a",
        .appIdentifier = APP_IDENTIFIER,
        .domain = "https://" + HOST,
        .status = InnerVerifyStatus::STATE_SUCCESS };
    RdbDataItem item2 = { .bundleName = "a",
        .appIdentifier = APP_IDENTIFIER,
        .domain = "https://",
        .status = InnerVerifyStatus::STATE_FAIL };
    items.emplace_back(item1);
    items.emplace_back(item2);
    dataMap.emplace("a", items);
    return true;
}

bool InvokeQueryAllDataBatchData(std::unordered_map<std::string, std::vector<RdbDataItem>>& dataMap)
{
    std::vector<RdbDataItem> items;
    RdbDataItem item1 = { .bundleName = "a",
        .appIdentifier = APP_IDENTIFIER,
        .domain = "https://" + HOST,
        .status = InnerVerifyStatus::STATE_SUCCESS };
    RdbDataItem item2 = { .bundleName = "a",
        .appIdentifier = APP_IDENTIFIER,
        .domain = "https://",
        .status = InnerVerifyStatus::STATE_FAIL };
    items.emplace_back(item1);
    items.emplace_back(item2);
    dataMap.emplace("a", items);
    dataMap.emplace("b", items);
    return true;
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

/**
 * @tc.name: MgrDataMgrVerifyResultInfoToDBTest001
 * @tc.desc: DataMgr test insert false
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrVerifyResultInfoToDBTest001, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, InsertData(_)).Times(1).WillOnce(Return(false));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = manager;
    std::string bundleName = "com.openharmony.com";
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = APP_IDENTIFIER;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("www.openharmony.com", InnerVerifyStatus::STATE_SUCCESS);
    EXPECT_FALSE(appDomainVerifyDataMgr->VerifyResultInfoToDB(bundleName, verifyResultInfo));
}
/**
 * @tc.name: MgrDataMgrVerifyResultInfoToDBTest002
 * @tc.desc: DataMgr test ok
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrVerifyResultInfoToDBTest002, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, InsertData(_)).Times(1).WillOnce(Return(true));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = manager;
    std::string bundleName = "com.openharmony.com";
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = APP_IDENTIFIER;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("www.openharmony.com", InnerVerifyStatus::STATE_SUCCESS);
    EXPECT_TRUE(appDomainVerifyDataMgr->VerifyResultInfoToDB(bundleName, verifyResultInfo));
}
/**
 * @tc.name: MgrDataMgrDBToVerifyResultInfoTest001
 * @tc.desc: DataMgr test items null
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrDBToVerifyResultInfoTest001, TestSize.Level0)
{
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    std::vector<RdbDataItem> items;
    VerifyResultInfo verifyResultInfo;
    EXPECT_FALSE(appDomainVerifyDataMgr->DBToVerifyResultInfo(items, verifyResultInfo));
}
/**
 * @tc.name: MgrDataMgrDBToVerifyResultInfoTest002
 * @tc.desc: DataMgr test ok
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrDBToVerifyResultInfoTest002, TestSize.Level0)
{
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    std::vector<RdbDataItem> items;
    RdbDataItem item1 = { .bundleName = "a",
        .appIdentifier = APP_IDENTIFIER,
        .domain = "https://" + HOST,
        .status = InnerVerifyStatus::STATE_SUCCESS };
    RdbDataItem item2 = { .bundleName = "a",
        .appIdentifier = APP_IDENTIFIER,
        .domain = "https://",
        .status = InnerVerifyStatus::STATE_FAIL };
    items.emplace_back(item1);
    items.emplace_back(item2);
    VerifyResultInfo verifyResultInfo;
    EXPECT_TRUE(appDomainVerifyDataMgr->DBToVerifyResultInfo(items, verifyResultInfo));
}
/**
 * @tc.name: MgrDataMgrSaveVerifyStatusTest001
 * @tc.desc: DataMgr test manager null
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrSaveVerifyStatusTest001, TestSize.Level0)
{
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = nullptr;
    std::string bundleName = "";
    VerifyResultInfo verifyResultInfo;
    ASSERT_FALSE(appDomainVerifyDataMgr->SaveVerifyStatus(bundleName, verifyResultInfo));
}
/**
 * @tc.name: MgrDataMgrSaveVerifyStatusTest002
 * @tc.desc: DataMgr test insert false
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrSaveVerifyStatusTest002, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, InsertData(_)).Times(1).WillOnce(Return(false));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = manager;
    std::string bundleName = "com.openharmony.com";
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = APP_IDENTIFIER;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("www.openharmony.com", InnerVerifyStatus::STATE_SUCCESS);
    ASSERT_FALSE(appDomainVerifyDataMgr->SaveVerifyStatus(bundleName, verifyResultInfo));
}
/**
 * @tc.name: MgrDataMgrSaveVerifyStatusTest003
 * @tc.desc: DataMgr test ok
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrSaveVerifyStatusTest003, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, InsertData(_)).Times(1).WillOnce(Return(true));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = manager;
    std::string bundleName = "com.openharmony.com";
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = APP_IDENTIFIER;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("www.openharmony.com", InnerVerifyStatus::STATE_SUCCESS);
    ASSERT_TRUE(appDomainVerifyDataMgr->SaveVerifyStatus(bundleName, verifyResultInfo));
}

/**
 * @tc.name: MgrDataMgrDeleteVerifyStatusTest001
 * @tc.desc: DataMgr test manager null
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrDeleteVerifyStatusTest001, TestSize.Level0)
{
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = nullptr;
    std::string bundleName = "com.openharmony.com";
    ASSERT_FALSE(appDomainVerifyDataMgr->DeleteVerifyStatus(bundleName));
}
/**
 * @tc.name: MgrDataMgrDeleteVerifyStatusTest002
 * @tc.desc: DataMgr test insert false
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrDeleteVerifyStatusTest002, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, DeleteData(_)).Times(1).WillOnce(Return(false));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = manager;
    std::string bundleName = "com.openharmony.com";
    ASSERT_FALSE(appDomainVerifyDataMgr->DeleteVerifyStatus(bundleName));
}
/**
 * @tc.name: MgrDataMgrDeleteVerifyStatusTest003
 * @tc.desc: DataMgr test ok
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrDeleteVerifyStatusTest003, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, DeleteData(_)).Times(1).WillOnce(Return(true));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = manager;
    std::string bundleName = "com.openharmony.com";
    ASSERT_TRUE(appDomainVerifyDataMgr->DeleteVerifyStatus(bundleName));
}
/**
 * @tc.name: MgrDataMgrDeleteVerifyStatusTest004
 * @tc.desc: DataMgr test name null
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrDeleteVerifyStatusTest004, TestSize.Level0)
{
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = nullptr;
    std::string bundleName = "";
    VerifyResultInfo verifyResultInfo;
    ASSERT_FALSE(appDomainVerifyDataMgr->DeleteVerifyStatus(bundleName));
}

/**
 * @tc.name: MgrDataMgrGetParamKeyTest001
 * @tc.desc: DataMgr test name empty
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrGetParamKeyTest001, TestSize.Level0)
{
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = nullptr;
    std::string bundleName = "";
    std::string paramKey = "";
    EXPECT_FALSE(appDomainVerifyDataMgr->GetParamKey(bundleName, paramKey));
}
/**
 * @tc.name: MgrDataMgrGetParamKeyTest002
 * @tc.desc: DataMgr test ok
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrGetParamKeyTest002, TestSize.Level0)
{
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = nullptr;
    std::string bundleName = "com.openharmony.com";
    std::string paramKey = "";
    EXPECT_TRUE(appDomainVerifyDataMgr->GetParamKey(bundleName, paramKey));
}
/**
 * @tc.name: MgrDataMgrLoadAllFromRdbTest001
 * @tc.desc: DataMgr test query failed
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrLoadAllFromRdbTest001, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, QueryAllData(_)).Times(1).WillOnce(Return(false));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    ASSERT_FALSE(appDomainVerifyDataMgr->LoadAllFromRdb());
}
/**
 * @tc.name: MgrDataMgrLoadAllFromRdbTest002
 * @tc.desc: DataMgr test query empty
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrLoadAllFromRdbTest002, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, QueryAllData(_)).Times(1).WillOnce(Return(true));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    ASSERT_TRUE(appDomainVerifyDataMgr->LoadAllFromRdb());
}

/**
 * @tc.name: MgrDataMgrLoadAllFromRdbTest003
 * @tc.desc: DataMgr query invoke wrong data
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrLoadAllFromRdbTest003, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, QueryAllData(_)).Times(1).WillOnce(Invoke(InvokeQueryAllDataWrongData));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = manager;
    ASSERT_FALSE(appDomainVerifyDataMgr->LoadAllFromRdb());
}
/**
 * @tc.name: MgrDataMgrLoadAllFromRdbTest004
 * @tc.desc: DataMgr query invoke data
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrLoadAllFromRdbTest004, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, QueryAllData(_)).Times(1).WillOnce(Invoke(InvokeQueryAllData));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = manager;
    ASSERT_TRUE(appDomainVerifyDataMgr->LoadAllFromRdb());
}
/**
 * @tc.name: MgrDataMgrLoadAllFromRdbTest005
 * @tc.desc: DataMgr query invoke data
 * @tc.type: FUNC
 */
HWTEST_F(MgrDataMgrTest, MgrDataMgrLoadAllFromRdbTest005, TestSize.Level0)
{
    MOC::impl = std::make_shared<MocAppDomainVerifyRdbDataManagerImpl>();
    AppDomainVerifyRdbConfig rdbConfig;
    std::shared_ptr<AppDomainVerifyRdbDataManager> manager = std::make_shared<AppDomainVerifyRdbDataManager>(rdbConfig);
    EXPECT_CALL(*MOC::impl, QueryAllData(_)).Times(1).WillOnce(Invoke(InvokeQueryAllDataBatchData));
    auto appDomainVerifyDataMgr = std::make_shared<AppDomainVerifyDataMgr>();
    appDomainVerifyDataMgr->rdbDataManager_ = manager;
    ASSERT_TRUE(appDomainVerifyDataMgr->LoadAllFromRdb());
}

}