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
#include "filter_define.h"
#include "app_details_filter.h"
#include "skill.h"
#include "gtest/gtest.h"
#include <memory>
#include <thread>
#define private public
#define protected public
#include "lru_cache_util.h"
#undef private
#undef protected

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
class AppDetailsFilterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AppDetailsFilterTest::SetUpTestCase(void)
{
}

void AppDetailsFilterTest::TearDownTestCase(void)
{
}

void AppDetailsFilterTest::SetUp(void)
{
}

void AppDetailsFilterTest::TearDown(void)
{
}

/**
 * @tc.name: AppDetailsFilterTest001
 * @tc.desc: test path match
 * @tc.type: FUNC
 */
HWTEST_F(AppDetailsFilterTest, AppDetailsFilterTest001, TestSize.Level0)
{
    AppDetailsFilter filter;
    std::vector<AppDetailInfo> filterVec;
    AppDetailInfo info;
    info.skillUri.scheme = "https";
    info.skillUri.host = "www.example.com";
    info.skillUri.path = "test";
    info.bundleName = "com.example.bundleName";
    filterVec.emplace_back(std::move(info));
    std::vector<AppDetailInfo> resultVec;
    auto ret = filter.Filter(filterVec, resultVec, "https://www.example.com/test");
    ASSERT_TRUE(ret);
    ASSERT_TRUE(resultVec.size() == 1);
    ASSERT_TRUE(resultVec[0].bundleName.compare("com.example.bundleName") == 0);
    resultVec.clear();
    filterVec[0].skillUri.path = "111";
    ret = filter.Filter(filterVec, resultVec, "https://www.example.com/test");
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: AppDetailsFilterTest002
 * @tc.desc: test path match
 * @tc.type: FUNC
 */
HWTEST_F(AppDetailsFilterTest, AppDetailsFilterTest002, TestSize.Level0)
{
    AppDetailsFilter filter;
    std::vector<AppDetailInfo> filterVec;
    AppDetailInfo info;
    info.skillUri.scheme = "https";
    info.skillUri.host = "www.example.com";
    info.skillUri.pathStartWith = "test";
    info.bundleName = "com.example.bundleName";
    filterVec.emplace_back(std::move(info));
    std::vector<AppDetailInfo> resultVec;
    auto ret = filter.Filter(filterVec, resultVec, "https://www.example.com/testhappy");
    ASSERT_TRUE(ret);
    ASSERT_TRUE(resultVec.size() == 1);
    ASSERT_EQ(resultVec[0].bundleName.compare("com.example.bundleName"), 0);
    resultVec.clear();
    filterVec[0].skillUri.path = "111";
    ret = filter.Filter(filterVec, resultVec, "https://www.example.com/tes");
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: AppDetailsFilterTest003
 * @tc.desc: test regex match
 * @tc.type: FUNC
 */
HWTEST_F(AppDetailsFilterTest, AppDetailsFilterTest003, TestSize.Level0)
{
    AppDetailsFilter filter;
    std::vector<AppDetailInfo> filterVec;
    AppDetailInfo info;
    info.skillUri.scheme = "https";
    info.skillUri.host = "www.example.com";
    info.skillUri.pathRegex = "[0-9]*";
    info.bundleName = "com.example.bundleName";
    filterVec.emplace_back(std::move(info));
    std::vector<AppDetailInfo> resultVec;
    auto ret = filter.Filter(filterVec, resultVec, "https://www.example.com/1223");
    ASSERT_TRUE(ret);
    ASSERT_TRUE(resultVec.size() == 1);
    ASSERT_EQ(resultVec[0].bundleName.compare("com.example.bundleName"), 0);
    ret = filter.Filter(filterVec, resultVec, "https://www.example.com/xxx");
    ASSERT_FALSE(ret);
}


}