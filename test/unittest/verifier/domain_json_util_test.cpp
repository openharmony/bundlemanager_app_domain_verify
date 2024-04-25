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
#include "domain_json_util.h"
#undef private
#undef protected
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class JsonUtilTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void JsonUtilTest::SetUpTestCase(void)
{
}

void JsonUtilTest::TearDownTestCase(void)
{
}

void JsonUtilTest::SetUp(void)
{
}

void JsonUtilTest::TearDown(void)
{
}
/**
 * @tc.name: JsonUtilTest001
 * @tc.desc: Parse test
 * @tc.type: FUNC
 */
HWTEST_F(JsonUtilTest, JsonUtilTest001, TestSize.Level0)
{
    // empty
    AssetJsonObj assetJsonObj;
    ASSERT_FALSE(JsonUtil::Parse("", assetJsonObj));

    // parse exception
    ASSERT_FALSE(JsonUtil::Parse("123", assetJsonObj));

    // obj null
    std::string assetJsonsStr = R"({})";
    ASSERT_FALSE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // obj not obj
    assetJsonsStr = R"({"applinking":1})";
    ASSERT_FALSE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // applinking null
    assetJsonsStr = R"({"applinkings":1})";
    ASSERT_FALSE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // applinking not obj
    assetJsonsStr = R"({"applinking":1})";
    ASSERT_FALSE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // apps null
    assetJsonsStr = R"({"applinking":{})";
    ASSERT_FALSE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // apps not array
    assetJsonsStr = R"({"applinking":{"apps":123})";
    ASSERT_FALSE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // appIdentifier null
    assetJsonsStr = R"({"applinking":{"apps":[{"bundleName":1,"fingerprint":1}]}})";
    ASSERT_TRUE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // bundleName null
    assetJsonsStr = R"({"applinking":{"apps":[{"appIdentifier":1,"fingerprint":1}]}})";
    ASSERT_TRUE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // fingerprint null
    assetJsonsStr = R"({"applinking":{"apps":[{"appIdentifier":1,"bundleName":1,}]}})";
    ASSERT_FALSE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // appIdentifier null
    assetJsonsStr = R"({"applinking":{"apps":[{"bundleName":"bundleName","fingerprint":"fingerprint"}]}})";
    ASSERT_TRUE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // bundleName null
    assetJsonsStr = R"({"applinking":{"apps":[{"appIdentifier":"appIdentifier","fingerprint":"fingerprint"}]}})";
    ASSERT_TRUE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // fingerprint null
    assetJsonsStr = R"({"applinking":{"apps":[{"appIdentifier":"appIdentifier","bundleName":"bundleName"}]}})";
    ASSERT_TRUE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // all
    assetJsonsStr = R"({"applinking":{"apps":[{"appIdentifier":1,"bundleName":1,"fingerprint":1}]}})";
    ASSERT_TRUE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));

    // all
    assetJsonsStr =
        R"({"applinking":{"apps":[{"appIdentifier":"appIdentifier","bundleName":"bundleName","fingerprint":"fingerprint"}]}})";
    ASSERT_TRUE(JsonUtil::Parse(assetJsonsStr, assetJsonObj));
}
}