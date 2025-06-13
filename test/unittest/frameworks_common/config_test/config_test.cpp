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
#include "white_list_config_mgr.h"
#undef private
#undef protected
#include "mock_preferences.h"
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
using namespace OHOS::NativePreferences;
class AppDomainVerifyConfigTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AppDomainVerifyConfigTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase";
}

void AppDomainVerifyConfigTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase";
}

void AppDomainVerifyConfigTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "SetUp";
}

void AppDomainVerifyConfigTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "TearDown";
    MockPreferences(nullptr);
}
const static std::string REG_DEFAULT_KEY = R"(^https:\/\/[a-zA-Z0-9-]+.openharmony.link\/123)";
const static std::string REG_DYNAMIC_KEY1 = R"(^https:\/\/[a-zA-Z0-9-]+.openharmony1.link\/123)";
const static std::string REG_DYNAMIC_KEY2 = R"(^https:\/\/[a-zA-Z0-9-]+.openharmony2.link\/123)";
const static std::string DEFAULT_URL = "https://atomic.openharmony.link/123";
const static std::string DYNAMIC_URL1 = "https://atomic1.openharmony1.link/123";
const static std::string DYNAMIC_URL2 = "https://atomic2.openharmony2.link/123";
const static std::string OTHER_URL = "https://atomic-other.openharmony3.link/123";
const static std::string REG_DEFAULT_URL_KEY = "regDefaultUrl";
const static std::string REG_WHITE_LIST_KEY = "regWhiteList";
const static std::string SPLITOR = ",";

/**
 * @tc.name: AppDomainVerifyCheckerTest001
 * @tc.desc: Checker test null preferences
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyCheckerTest001, TestSize.Level0)
{
    MockPreferences(nullptr);

    std::shared_ptr<WhiteListConfigMgr> checker = std::make_shared<WhiteListConfigMgr>();

    ASSERT_FALSE(checker->IsInWhiteList(OTHER_URL));
    ASSERT_FALSE(checker->IsInWhiteList(DEFAULT_URL));
    ASSERT_FALSE(checker->IsInWhiteList(DYNAMIC_URL1));
}
/**
 * @tc.name: AppDomainVerifyCheckerTest002
 * @tc.desc: Checker test empty preferences
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyCheckerTest002, TestSize.Level0)
{
    std::shared_ptr<MocPreferences> mocPreferences = std::make_shared<MocPreferences>();
    EXPECT_CALL(*mocPreferences, GetString(_, _)).Times(AtLeast(1)).WillRepeatedly(Return(""));
    MockPreferences(mocPreferences);

    std::shared_ptr<WhiteListConfigMgr> checker = std::make_shared<WhiteListConfigMgr>();

    ASSERT_FALSE(checker->IsInWhiteList(OTHER_URL));
    ASSERT_FALSE(checker->IsInWhiteList(DEFAULT_URL));
    ASSERT_FALSE(checker->IsInWhiteList(DYNAMIC_URL1));
}

/**
 * @tc.name: AppDomainVerifyCheckerTest003
 * @tc.desc: AddTask test, return default
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyCheckerTest003, TestSize.Level0)
{
    std::shared_ptr<MocPreferences> mocPreferences = std::make_shared<MocPreferences>();
    EXPECT_CALL(*mocPreferences, GetString(_, _)).Times(AtLeast(1)).WillRepeatedly(Return(REG_DEFAULT_KEY));
    MockPreferences(mocPreferences);

    std::shared_ptr<WhiteListConfigMgr> checker = std::make_shared<WhiteListConfigMgr>();

    ASSERT_FALSE(checker->IsInWhiteList(OTHER_URL));
    ASSERT_TRUE(checker->IsInWhiteList(DEFAULT_URL));
    ASSERT_FALSE(checker->IsInWhiteList(DYNAMIC_URL1));
}

/**
 * @tc.name: AppDomainVerifyCheckerTest004
 * @tc.desc: AddTask test, return dynamic
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyCheckerTest004, TestSize.Level0)
{
    std::shared_ptr<MocPreferences> mocPreferences = std::make_shared<MocPreferences>();
    EXPECT_CALL(*mocPreferences, GetString(REG_DEFAULT_URL_KEY, _)).Times(AtLeast(1)).WillOnce(Return(REG_DEFAULT_KEY));
    EXPECT_CALL(*mocPreferences, GetString(REG_WHITE_LIST_KEY, _)).Times(AtLeast(1)).WillOnce(Return(REG_DYNAMIC_KEY1));
    MockPreferences(mocPreferences);

    std::shared_ptr<WhiteListConfigMgr> checker = std::make_shared<WhiteListConfigMgr>();

    ASSERT_FALSE(checker->IsInWhiteList(OTHER_URL));
    ASSERT_TRUE(checker->IsInWhiteList(DYNAMIC_URL1));
    ASSERT_TRUE(checker->IsInWhiteList(DEFAULT_URL));
}
/**
 * @tc.name: AppDomainVerifyCheckerTest005
 * @tc.desc: AddTask test, return dynamic
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyCheckerTest005, TestSize.Level0)
{
    std::shared_ptr<MocPreferences> mocPreferences = std::make_shared<MocPreferences>();
    EXPECT_CALL(*mocPreferences, GetString(REG_DEFAULT_URL_KEY, _)).Times(AtLeast(1)).WillOnce(Return(REG_DEFAULT_KEY));
    EXPECT_CALL(*mocPreferences, GetString(REG_WHITE_LIST_KEY, _))
        .Times(AtLeast(1))
        .WillOnce(Return(REG_DYNAMIC_KEY1 + ","));
    MockPreferences(mocPreferences);

    std::shared_ptr<WhiteListConfigMgr> checker = std::make_shared<WhiteListConfigMgr>();

    ASSERT_FALSE(checker->IsInWhiteList(OTHER_URL));
    ASSERT_TRUE(checker->IsInWhiteList(DYNAMIC_URL1));
    ASSERT_TRUE(checker->IsInWhiteList(DEFAULT_URL));
}

/**
 * @tc.name: AppDomainVerifyCheckerTest006
 * @tc.desc: AddTask test, return dynamic
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyCheckerTest006, TestSize.Level0)
{
    std::shared_ptr<MocPreferences> mocPreferences = std::make_shared<MocPreferences>();
    EXPECT_CALL(*mocPreferences, GetString(REG_DEFAULT_URL_KEY, _)).Times(AtLeast(1)).WillOnce(Return(REG_DEFAULT_KEY));
    EXPECT_CALL(*mocPreferences, GetString(REG_WHITE_LIST_KEY, _))
        .Times(AtLeast(1))
        .WillOnce(Return(REG_DYNAMIC_KEY1 + "," + REG_DYNAMIC_KEY2));
    MockPreferences(mocPreferences);
    std::shared_ptr<WhiteListConfigMgr> checker = std::make_shared<WhiteListConfigMgr>();

    ASSERT_FALSE(checker->IsInWhiteList(OTHER_URL));
    ASSERT_TRUE(checker->IsInWhiteList(DEFAULT_URL));
    ASSERT_TRUE(checker->IsInWhiteList(DYNAMIC_URL2));
    ASSERT_TRUE(checker->IsInWhiteList(DYNAMIC_URL1));
}
/**
 * @tc.name: AppDomainVerifyUpdaterTest001
 * @tc.desc: AddTask test, return null ref
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyUpdaterTest001, TestSize.Level0)
{
    MockPreferences(nullptr);
    std::shared_ptr<WhiteListConfigMgr> updater = std::make_shared<WhiteListConfigMgr>();
    std::unordered_set<std::string> whiteList;

    updater->UpdateWhiteList(whiteList);

    ASSERT_TRUE(updater->whiteListSet_.empty());
}

/**
 * @tc.name: AppDomainVerifyUpdaterTest002
 * @tc.desc: AddTask test, return null ref
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyUpdaterTest002, TestSize.Level0)
{
    std::shared_ptr<MocPreferences> mocPreferences = std::make_shared<MocPreferences>();
    EXPECT_CALL(*mocPreferences, PutString(REG_WHITE_LIST_KEY, REG_DYNAMIC_KEY1 + ","))
        .Times(AtLeast(1))
        .WillOnce(Return(1));
    MockPreferences(mocPreferences);
    std::shared_ptr<WhiteListConfigMgr> updater = std::make_shared<WhiteListConfigMgr>();
    std::unordered_set<std::string> whiteList;
    whiteList.insert(REG_DYNAMIC_KEY1);

    updater->UpdateWhiteList(whiteList);

    ASSERT_TRUE(!updater->whiteListSet_.empty());
}

/**
 * @tc.name: AppDomainVerifyUpdaterTest003
 * @tc.desc: AddTask test, return ok
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyUpdaterTest003, TestSize.Level0)
{
    std::shared_ptr<MocPreferences> mocPreferences = std::make_shared<MocPreferences>();
    EXPECT_CALL(*mocPreferences, PutString(REG_WHITE_LIST_KEY, _)).Times(1).WillOnce(Return(0));
    MockPreferences(mocPreferences);
    std::shared_ptr<WhiteListConfigMgr> updater = std::make_shared<WhiteListConfigMgr>();
    std::unordered_set<std::string> whiteList;
    whiteList.insert(REG_DYNAMIC_KEY1);
    whiteList.insert(REG_DYNAMIC_KEY2);

    updater->UpdateWhiteList(whiteList);

    ASSERT_TRUE(!updater->whiteListSet_.empty());
}
/**
 * @tc.name: AppDomainVerifyUpdaterTest004
 * @tc.desc: AddTask test, return ok
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyUpdaterTest004, TestSize.Level0)
{
    std::shared_ptr<MocPreferences> mocPreferences = std::make_shared<MocPreferences>();
    EXPECT_CALL(*mocPreferences, PutString(REG_WHITE_LIST_KEY, _)).Times(1).WillOnce(Return(0));
    MockPreferences(mocPreferences);
    std::shared_ptr<WhiteListConfigMgr> updater = std::make_shared<WhiteListConfigMgr>();
    std::unordered_set<std::string> whiteList;
    whiteList.insert("  ");
    whiteList.insert(REG_DYNAMIC_KEY1);
    whiteList.insert(REG_DYNAMIC_KEY2);

    updater->UpdateWhiteList(whiteList);

    ASSERT_TRUE(!updater->whiteListSet_.empty());
}

/**
 * @tc.name: AppDomainVerifyUpdaterTest005
 * @tc.desc: AddTask test, empty whitelist
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyUpdaterTest005, TestSize.Level0)
{
    std::shared_ptr<MocPreferences> mocPreferences = std::make_shared<MocPreferences>();
    MockPreferences(mocPreferences);
    std::shared_ptr<WhiteListConfigMgr> updater = std::make_shared<WhiteListConfigMgr>();
    std::unordered_set<std::string> whiteList;
    whiteList.insert("");

    updater->UpdateWhiteList(whiteList);

    ASSERT_TRUE(updater->whiteListSet_.empty());
}
/**
 * @tc.name: AppDomainVerifyUpdaterTest006
 * @tc.desc: AddTask test, getPreference status err
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConfigTest, AppDomainVerifyUpdaterTest006, TestSize.Level0)
{
    std::shared_ptr<MocPreferences> mocPreferences = std::make_shared<MocPreferences>();
    MockPreferences(mocPreferences, -1);
    std::shared_ptr<WhiteListConfigMgr> updater = std::make_shared<WhiteListConfigMgr>();
    std::unordered_set<std::string> whiteList;
    whiteList.insert("");

    updater->UpdateWhiteList(whiteList);

    ASSERT_TRUE(updater->whiteListSet_.empty());
}

}