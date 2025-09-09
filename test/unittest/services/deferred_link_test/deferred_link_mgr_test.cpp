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
#include "gtest/gtest.h"
#include "mock_constant.h"
#define private public
#define protected public
#include "ability_filter.h"
#include "deferred_link_mgr.h"
#undef private
#undef protected
#include "mock_ability_filter.h"
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
constexpr const char* BUNDLE_NAME_WRONG = "BUNDLE_NAME_WRONG";
constexpr const char* BUNDLE_DOMAIN = "https://www.openharmony.cn";
constexpr const char* BUNDLE_DOMAIN_WRONG = "https://www.openharmony_wrong.cn";
constexpr const char* BUNDLE_URL = "https://www.openharmony.cn/100";
constexpr const char* BUNDLE_URL_NEW = "https://www.openharmony.cn/new";
constexpr int MAX_CACHE_SIZE = 50;
class DeferredLinkMgrTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DeferredLinkMgrTest::SetUpTestCase(void)
{
}

void DeferredLinkMgrTest::TearDownTestCase(void)
{
}

void DeferredLinkMgrTest::SetUp(void)
{
}

void DeferredLinkMgrTest::TearDown(void)
{
}

/**
 * @tc.name: DeferredLinkPutTest001
 * @tc.desc: put one link, remain one.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkPutTest001, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;

    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });

    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 1);
    EXPECT_TRUE(deferredLinkMgr.caches_.front().domain == BUNDLE_DOMAIN);
    EXPECT_TRUE(deferredLinkMgr.caches_.front().url == BUNDLE_URL);
}

/**
 * @tc.name: DeferredLinkPutTest002
 * @tc.desc: put max_size + 1 link, will remain maxsize.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkPutTest002, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;

    for (int i = 0; i < MAX_CACHE_SIZE + 1; i++) {
        deferredLinkMgr.PutDeferredLink({ .domain = BUNDLE_DOMAIN + std::to_string(i),
            .url = BUNDLE_URL,
            .timeStamp = GetSecondsSince1970ToNow() });
    }

    EXPECT_TRUE(deferredLinkMgr.caches_.size() == MAX_CACHE_SIZE);
    EXPECT_EQ(deferredLinkMgr.caches_.back().domain, BUNDLE_DOMAIN + std::to_string(1));
    EXPECT_EQ(deferredLinkMgr.caches_.front().domain, BUNDLE_DOMAIN + std::to_string(MAX_CACHE_SIZE));
}

/**
 * @tc.name: DeferredLinkPutTest003
 * @tc.desc: put duplicate link, will unique and in back.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkPutTest003, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;

    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN + std::to_string(1), .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });

    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 2);
    EXPECT_EQ(deferredLinkMgr.caches_.back().domain, BUNDLE_DOMAIN + std::to_string(1));
    EXPECT_EQ(deferredLinkMgr.caches_.front().domain, BUNDLE_DOMAIN);
}

/**
 * @tc.name: DeferredLinkGetTest001
 * @tc.desc: get link with matched domain and matched url, return link.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkGetTest001, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;
    std::shared_ptr<MocAbilityFilter> filter = std::make_shared<MocAbilityFilter>();
    EXPECT_CALL(*filter, Filter(_)).Times(1).WillOnce(Return(true));
    MockAbilityFilter(filter);
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });
    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 1);

    std::vector<std::string> domains;
    domains.emplace_back(BUNDLE_DOMAIN);
    auto link = deferredLinkMgr.GetDeferredLink(BUNDLE_NAME, domains);

    EXPECT_FALSE(link.empty());
    EXPECT_EQ(link, BUNDLE_URL);
    EXPECT_TRUE(deferredLinkMgr.caches_.empty());

    auto link1 = deferredLinkMgr.GetDeferredLink(BUNDLE_NAME, domains);
    EXPECT_TRUE(link1.empty());
}

/**
 * @tc.name: DeferredLinkGetTest002
 * @tc.desc: get link with unmatched domains and matched bundleName, return none.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkGetTest002, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;
    std::shared_ptr<MocAbilityFilter> filter = std::make_shared<MocAbilityFilter>();
    EXPECT_CALL(*filter, Filter(_)).Times(0);
    MockAbilityFilter(filter);
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });
    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 1);

    std::vector<std::string> domains;
    domains.emplace_back(BUNDLE_DOMAIN_WRONG);
    auto link = deferredLinkMgr.GetDeferredLink(BUNDLE_NAME, domains);

    EXPECT_TRUE(link.empty());
    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 1);
}

/**
 * @tc.name: DeferredLinkGetTest003
 * @tc.desc: get link with unmatched domains and matched bundleName, return none.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkGetTest003, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;
    std::shared_ptr<MocAbilityFilter> filter = std::make_shared<MocAbilityFilter>();
    EXPECT_CALL(*filter, Filter(_)).Times(0);
    MockAbilityFilter(filter);
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });
    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 1);

    std::vector<std::string> domains;
    domains.emplace_back(BUNDLE_DOMAIN_WRONG);
    auto link = deferredLinkMgr.GetDeferredLink(BUNDLE_NAME, domains);

    EXPECT_TRUE(link.empty());
    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 1);
}

/**
 * @tc.name: DeferredLinkGetTest004
 * @tc.desc: get link with unmatched bundleName and matched domains, return none.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkGetTest004, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;
    std::shared_ptr<MocAbilityFilter> filter = std::make_shared<MocAbilityFilter>();
    EXPECT_CALL(*filter, Filter(_)).Times(1).WillOnce(Return(false));
    MockAbilityFilter(filter);
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });
    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 1);

    std::vector<std::string> domains;
    domains.emplace_back(BUNDLE_DOMAIN);
    auto link = deferredLinkMgr.GetDeferredLink(BUNDLE_NAME_WRONG, domains);

    EXPECT_TRUE(link.empty());
    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 1);
}

/**
 * @tc.name: DeferredLinkGetTest005
 * @tc.desc: get link with matched domain and matched url, return new link, cache remove link all.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkGetTest005, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;
    std::shared_ptr<MocAbilityFilter> filter = std::make_shared<MocAbilityFilter>();
    EXPECT_CALL(*filter, Filter(_)).Times(2).WillRepeatedly(Return(true));
    MockAbilityFilter(filter);
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL_NEW, .timeStamp = GetSecondsSince1970ToNow() });
    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 2);

    std::vector<std::string> domains;
    domains.emplace_back(BUNDLE_DOMAIN);
    auto link = deferredLinkMgr.GetDeferredLink(BUNDLE_NAME, domains);

    EXPECT_FALSE(link.empty());
    EXPECT_EQ(link, BUNDLE_URL_NEW);
    EXPECT_TRUE(deferredLinkMgr.caches_.empty());

    auto link1 = deferredLinkMgr.GetDeferredLink(BUNDLE_NAME, domains);
    EXPECT_TRUE(link1.empty());
}

/**
 * @tc.name: DeferredLinkGetTest006
 * @tc.desc: get link from empty cache, return null.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkGetTest006, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;
    std::shared_ptr<MocAbilityFilter> filter = std::make_shared<MocAbilityFilter>();
    MockAbilityFilter(filter);

    std::vector<std::string> domains;
    domains.emplace_back(BUNDLE_DOMAIN);
    auto link = deferredLinkMgr.GetDeferredLink(BUNDLE_NAME, domains);

    EXPECT_TRUE(link.empty());
}
/**
 * @tc.name: DeferredLinkGetTest007
 * @tc.desc: age process.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkGetTest007, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() - 10 * 60 });
    deferredLinkMgr.AgeCacheProcess();
    ASSERT_TRUE(deferredLinkMgr.caches_.empty());
}
/**
 * @tc.name: DeferredLinkGetTest008
 * @tc.desc: age process.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkGetTest008, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });
    deferredLinkMgr.AgeCacheProcess();
    ASSERT_FALSE(deferredLinkMgr.caches_.empty());
}
/**
 * @tc.name: DeferredLinkGetTest009
 * @tc.desc: remove deferred link, so get empty link.
 * @tc.type: FUNC
 */
HWTEST_F(DeferredLinkMgrTest, DeferredLinkGetTest009, TestSize.Level0)
{
    DeferredLinkMgr deferredLinkMgr;
    deferredLinkMgr.ageHandler_ = nullptr;
    deferredLinkMgr.PutDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });
    EXPECT_TRUE(deferredLinkMgr.caches_.size() == 1);

    deferredLinkMgr.RemoveDeferredLink(
        { .domain = BUNDLE_DOMAIN, .url = BUNDLE_URL, .timeStamp = GetSecondsSince1970ToNow() });
    EXPECT_TRUE(deferredLinkMgr.caches_.empty());

    std::vector<std::string> domains;
    domains.emplace_back(BUNDLE_DOMAIN);
    auto link = deferredLinkMgr.GetDeferredLink(BUNDLE_NAME, domains);

    EXPECT_TRUE(link.empty());
}
}