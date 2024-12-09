/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "app_details_meta_item.h"
#include "app_details_rdb_item.h"
#include "cpp/condition_variable.h"
#include "cpp/mutex.h"
#include "cpp/task.h"
#include "skill.h"
#include "gtest/gtest.h"
#include <chrono>
#include <memory>
#include <mutex>
#include <semaphore>
#include <string>
#include <thread>
#include <vector>
#include "app_domain_verify_hilog.h"
#define private public
#define protected public
#include "app_details_rdb_data_manager.h"
#undef private
#undef protected

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
class AppDetailsRdbMgrTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static std::string rdbName;
    static std::string rdbPath;
    static int rdbVersion;
};

std::string AppDetailsRdbMgrTest::rdbName = "test.db";
std::string AppDetailsRdbMgrTest::rdbPath = "/data/test/";
int AppDetailsRdbMgrTest::rdbVersion = 1;

void AppDetailsRdbMgrTest::SetUpTestCase(void)
{
}

void AppDetailsRdbMgrTest::TearDownTestCase(void)
{
}

void AppDetailsRdbMgrTest::SetUp(void)
{
}

void AppDetailsRdbMgrTest::TearDown(void)
{
    std::vector<std::string> tmpFileVec = {
        rdbPath + rdbName,
        rdbPath + rdbName + "-shm",
        rdbPath + rdbName + "-wal"
    };
    for (auto& filePath : tmpFileVec) {
        (void)remove(filePath.c_str());
    }
}

/**
 * @tc.name: AppDetailsRdbMgrTest001
 * @tc.desc: test table op
 * @tc.type: FUNC
 */
HWTEST_F(AppDetailsRdbMgrTest, AppDetailsRdbMgrTest001, TestSize.Level0)
{
    RdbConfigInfo info;
    info.rdbName = AppDetailsRdbMgrTest::rdbName;
    info.rdbPath = AppDetailsRdbMgrTest::rdbPath;
    info.version = AppDetailsRdbMgrTest::rdbVersion;
    auto rdbMgr = std::make_shared<AppDetailsRdbDataMgr>(info);
    bool ret = rdbMgr->DeleteTable("app_details");
    ASSERT_TRUE(ret);
    ret = rdbMgr->CreateTable("app_details");
    ASSERT_TRUE(ret);
    ret = rdbMgr->CreateTable("app_details_tmp");
    ASSERT_TRUE(ret);
    ret = rdbMgr->DeleteTable("app_details_del");
    ASSERT_TRUE(ret);
    ret = rdbMgr->RenameTable("app_details", "app_details_del");
    ASSERT_TRUE(ret);
    ret = rdbMgr->DeleteTable("app_details_del");
    ASSERT_TRUE(ret);
    ret = rdbMgr->DeleteTable("app_details_tmp");
    ASSERT_TRUE(ret);
}


/**
 * @tc.name: AppDetailsRdbMgrTest002
 * @tc.desc: test insert del query op
 * @tc.type: FUNC
 */
HWTEST_F(AppDetailsRdbMgrTest, AppDetailsRdbMgrTest002, TestSize.Level0)
{
    RdbConfigInfo info;
    info.rdbName = AppDetailsRdbMgrTest::rdbName;
    info.rdbPath = AppDetailsRdbMgrTest::rdbPath;
    info.version = AppDetailsRdbMgrTest::rdbVersion;
    auto rdbMgr = std::make_shared<AppDetailsRdbDataMgr>(info);
    bool ret = rdbMgr->DeleteTable("app_details");
    ASSERT_TRUE(ret);
    ret = rdbMgr->CreateTable("app_details");
    ASSERT_TRUE(ret);
    std::vector<AppDetailsRdbItem> itemVec;
    AppDetailsRdbItem item;
    item.id = "111";
    item.scheme = "https";
    item.domain = "www.test.com";
    item.pathType = "path";
    item.path = "test";
    item.bundleName = "com.huawei.bundleNameTest";
    itemVec.push_back(item);
    ret = rdbMgr->InsertDataBatch("app_details", itemVec);
    ASSERT_TRUE(ret);
    std::vector<AppDetailsRdbItem> itemRetVec;
    ret = rdbMgr->QueryDataByDomain("app_details", item.domain, itemRetVec);
    ASSERT_TRUE(ret);
    ASSERT_TRUE(itemRetVec.size() != 0);
    auto rdbitem = itemRetVec[0];
    ASSERT_EQ(rdbitem.bundleName, item.bundleName);
    ret = rdbMgr->DeleteTable("app_details");
    ASSERT_TRUE(ret);
}


/**
 * @tc.name: AppDetailsRdbMgrTest003
 * @tc.desc: test meta data
 * @tc.type: FUNC
 */
HWTEST_F(AppDetailsRdbMgrTest, AppDetailsRdbMgrTest003, TestSize.Level0)
{
    RdbConfigInfo info;
    info.rdbName = AppDetailsRdbMgrTest::rdbName;
    info.rdbPath = AppDetailsRdbMgrTest::rdbPath;
    info.version = AppDetailsRdbMgrTest::rdbVersion;
    auto rdbMgr = std::make_shared<AppDetailsRdbDataMgr>(info);
    bool ret = rdbMgr->CreateMetaData();
    ASSERT_TRUE(ret);
    ret = rdbMgr->CreateMetaData();
    ASSERT_TRUE(ret);
    MetaItem item;
    item.tableExtInfo = "tableExtInfo";
    item.tableName = "app_details";
    item.tableVersion = "1.0";
    item.updateTime = "111111";
    std::vector<MetaItem> itemVec;
    itemVec.push_back(item);
    ret = rdbMgr->UpdateMetaData(itemVec);
    ASSERT_TRUE(ret);
    ret = rdbMgr->DeleteTable("META_DATA");
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: AppDetailsRdbMgrTest004
 * @tc.desc: test transaction
 * @tc.type: FUNC
 */
HWTEST_F(AppDetailsRdbMgrTest, AppDetailsRdbMgrTest004, TestSize.Level0)
{
    RdbConfigInfo info;
    info.rdbName = AppDetailsRdbMgrTest::rdbName;
    info.rdbPath = AppDetailsRdbMgrTest::rdbPath;
    info.version = AppDetailsRdbMgrTest::rdbVersion;
    auto rdbMgr = std::make_shared<AppDetailsRdbDataMgr>(info);
    auto ret = rdbMgr->CreateTable("app_details");
    std::vector<AppDetailsRdbItem> itemVec;
    AppDetailsRdbItem item;
    item.id = "111";
    item.scheme = "https";
    item.domain = "www.test.com";
    item.pathType = "path";
    item.path = "test";
    item.bundleName = "com.huawei.bundleNameTest";
    itemVec.push_back(item);
    ret = rdbMgr->InsertDataBatch("app_details", itemVec);
    ASSERT_TRUE(ret);
    bool retFlag = rdbMgr->ExecWithTrans([&]()->bool {
        auto ret = rdbMgr->DeleteTable("app_details");
        return false;
    });
    ASSERT_FALSE(retFlag);
    std::vector<AppDetailsRdbItem> result;
    ret = rdbMgr->QueryDataByDomain("app_details", item.domain, result);
    ASSERT_TRUE(ret);
    retFlag = rdbMgr->ExecWithTrans([&]()->bool {
        auto ret = rdbMgr->DeleteTable("app_details");
        return true;
    });
    ASSERT_TRUE(retFlag);
    result.clear();
    ret = rdbMgr->QueryDataByDomain("app_details", item.domain, result);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: AppDetailsRdbMgrTest005
 * @tc.desc: test delayClose
 * @tc.type: FUNC
 */
HWTEST_F(AppDetailsRdbMgrTest, AppDetailsRdbMgrTest005, TestSize.Level0)
{
    RdbConfigInfo info;
    info.rdbName = AppDetailsRdbMgrTest::rdbName;
    info.rdbPath = AppDetailsRdbMgrTest::rdbPath;
    info.version = AppDetailsRdbMgrTest::rdbVersion;
    auto rdbMgr = std::make_shared<AppDetailsRdbDataMgr>(info);
    auto rdbStore = rdbMgr->GetRdbStore();
    ASSERT_TRUE(rdbStore != nullptr);
    rdbMgr->PostDelayCloseTask(100);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    ASSERT_TRUE(rdbMgr->rdbStore_ == nullptr);
}

/**
 * @tc.name: AppDetailsRdbMgrTest006
 * @tc.desc: test trans for muti table
 * @tc.type: FUNC
 */
HWTEST_F(AppDetailsRdbMgrTest, AppDetailsRdbMgrTest006, TestSize.Level0)
{
    RdbConfigInfo info;
    info.rdbName = AppDetailsRdbMgrTest::rdbName;
    info.rdbPath = AppDetailsRdbMgrTest::rdbPath;
    info.version = AppDetailsRdbMgrTest::rdbVersion;
    auto rdbMgr = std::make_shared<AppDetailsRdbDataMgr>(info);
    auto ret = rdbMgr->CreateTable("app_details");
    rdbMgr->closeHandler_->RemoveTask("closeRdb");
    std::vector<AppDetailsRdbItem> itemVec;
    AppDetailsRdbItem item;
    item.id = "111";
    item.scheme = "https";
    item.domain = "www.test.com";
    item.pathType = "path";
    item.path = "test";
    item.bundleName = "com.test.bundleNameTest";
    itemVec.push_back(item);
    ret = rdbMgr->InsertDataBatch("app_details", itemVec);
    std::vector<AppDetailsRdbItem> result;
    std::atomic_bool checkFlag = false;
    std::binary_semaphore sem(0);
    ret = rdbMgr->QueryDataByDomain("app_details", item.domain, result);
    ASSERT_TRUE(ret);
    ASSERT_TRUE(result.size() != 0);
    auto task = [&sem, &checkFlag]() {
        rdbMgr->ExecWithTrans([&sem, &checkFlag]()->bool {
            sem.acquire();
            checkFlag = true;
            return true;
        });
        return;
    };
    rdbMgr->closeHandler_->PostTask(task, "closeRdb", 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    bool flag = rdbMgr->rdbStore_->IsInTransaction();
    ASSERT_TRUE(flag);
    ASSERT_FALSE(checkFlag);
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "sart query test");
    result.clear();
    ASSERT_TRUE(result.size() == 0);
    ret = rdbMgr->QueryDataByDomain("app_details", "www.baidu.com", result);
    ASSERT_TRUE(result.size() != 0);
    sem.release();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(checkFlag);
}
}