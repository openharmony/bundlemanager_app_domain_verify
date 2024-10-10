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
#include "mock_constant.h"
#define private public
#define protected public
#include "skill_uri.h"
#include "app_verify_base_info.h"
#include "bundle_verify_status_info.h"
#include "domain_url_util.h"
#undef private
#undef protected
#include "common_utils.h"

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class AppDomainVerifyBeanTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};
void AppDomainVerifyBeanTest::SetUpTestCase(void)
{
}

void AppDomainVerifyBeanTest::TearDownTestCase(void)
{
}

void AppDomainVerifyBeanTest::SetUp(void)
{
}

void AppDomainVerifyBeanTest::TearDown(void)
{
}
/**
 * @tc.name: AppDomainVerifySkillUriTest001
 * @tc.desc: SkillUri test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanTest, AppDomainVerifySkillUriTest001, TestSize.Level0)
{
    SkillUri skillUri;
    skillUri.scheme = "scheme";
    skillUri.host = "host";
    skillUri.port = "port";
    skillUri.path = "path";
    skillUri.pathStartWith = "pathStartWith";
    skillUri.pathRegex = "pathRegex";
    skillUri.type = "type";
    Parcel parcel;
    skillUri.Marshalling(parcel);
    auto unmarshalling = SkillUri::Unmarshalling(parcel);
    ASSERT_TRUE(unmarshalling->scheme == "scheme");
    ASSERT_TRUE(unmarshalling->host == "host");
    ASSERT_TRUE(unmarshalling->port == "port");
    ASSERT_TRUE(unmarshalling->path == "path");
    ASSERT_TRUE(unmarshalling->pathStartWith == "pathStartWith");
    ASSERT_TRUE(unmarshalling->pathRegex == "pathRegex");
    ASSERT_TRUE(unmarshalling->type == "type");

    Parcel parcel1;
    unmarshalling = SkillUri::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);

    parcel1.WriteString("https");
    unmarshalling = SkillUri::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
}
/**
 * @tc.name: AppDomainVerifyAppVerifyBaseInfoTest001
 * @tc.desc: AppVerifyBaseInfo test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanTest, AppDomainVerifyAppVerifyBaseInfoTest001, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.appIdentifier = "appIdentifier";
    appVerifyBaseInfo.bundleName = "bundleName";
    appVerifyBaseInfo.fingerprint = "fingerprint";

    Parcel parcel;
    appVerifyBaseInfo.Marshalling(parcel);
    auto unmarshalling = AppVerifyBaseInfo::Unmarshalling(parcel);
    ASSERT_TRUE(unmarshalling->appIdentifier == "appIdentifier");
    ASSERT_TRUE(unmarshalling->bundleName == "bundleName");
    ASSERT_TRUE(unmarshalling->fingerprint == "fingerprint");

    Parcel parcel1;
    unmarshalling = AppVerifyBaseInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteString(APP_IDENTIFIER);
    unmarshalling = AppVerifyBaseInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteString(BUNDLE_NAME);
    unmarshalling = AppVerifyBaseInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
}
/**
 * @tc.name: AppDomainVerifyBundleVerifyStatusInfoTest001
 * @tc.desc: BundleVerifyStatusInfo test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanTest, AppDomainVerifyBundleVerifyStatusInfoTest001, TestSize.Level0)
{
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, "", 0));
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.insert_or_assign(BUNDLE_NAME, verifyResultInfo);

    Parcel parcel;
    bundleVerifyStatusInfo.Marshalling(parcel);
    auto unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel);

    ASSERT_TRUE(unmarshalling->bundleVerifyStatusInfoMap_.size() == 1);
    ASSERT_TRUE(unmarshalling->bundleVerifyStatusInfoMap_.begin()->first == BUNDLE_NAME);
    ASSERT_TRUE(unmarshalling->bundleVerifyStatusInfoMap_.begin()->second.hostVerifyStatusMap.size() == 1);
    ASSERT_TRUE(unmarshalling->bundleVerifyStatusInfoMap_.begin()->second.hostVerifyStatusMap.begin()->first ==
        "https://" + HOST);
    auto [status, verifyTime, cnt] = unmarshalling->bundleVerifyStatusInfoMap_.begin()->second.hostVerifyStatusMap.begin()->second;
    ASSERT_TRUE(status == InnerVerifyStatus::STATE_SUCCESS);

    Parcel parcel1;
    unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteUint32(1);
    unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteString(BUNDLE_NAME);
    unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteString(APP_IDENTIFIER);
    unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteUint32(1);
    unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteString("https://" + HOST);
    unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
}
/**
 * @tc.name: AppDomainVerifyBundleVerifyStatusInfoTest002
 * @tc.desc: BundleVerifyStatusInfo test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanTest, AppDomainVerifyBundleVerifyStatusInfoTest002, TestSize.Level0)
{
    Parcel parcel1;
    auto unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteUint32(1);
    unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteString(BUNDLE_NAME);
    unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteString(APP_IDENTIFIER);
    unmarshalling = BundleVerifyStatusInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteUint32(100);
}
/**
 * @tc.name: AppDomainVerifyVerifyResultInfoTest001
 * @tc.desc: VerifyResultInfo test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanTest, AppDomainVerifyVerifyResultInfoTest001, TestSize.Level0)
{
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = APP_IDENTIFIER;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, "", 0));

    Parcel parcel;
    verifyResultInfo.Marshalling(parcel);
    auto unmarshalling = VerifyResultInfo::Unmarshalling(parcel);
    ASSERT_TRUE(unmarshalling->appIdentifier == APP_IDENTIFIER);
    ASSERT_TRUE(unmarshalling->hostVerifyStatusMap.size() == 1);
    ASSERT_TRUE(unmarshalling->hostVerifyStatusMap.begin()->first == "https://" + HOST);
    auto [status, verifyTime, cnt] = unmarshalling->hostVerifyStatusMap.begin()->second;
    ASSERT_TRUE(status == InnerVerifyStatus::STATE_SUCCESS);

    Parcel parcel1;
    unmarshalling = VerifyResultInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteString(APP_IDENTIFIER);
    unmarshalling = VerifyResultInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteUint32(1);
    unmarshalling = VerifyResultInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
}
/**
 * @tc.name: AppDomainVerifyVerifyResultInfoTest002
 * @tc.desc: VerifyResultInfo test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanTest, AppDomainVerifyVerifyResultInfoTest002, TestSize.Level0)
{
    Parcel parcel1;
    auto unmarshalling = VerifyResultInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteString(APP_IDENTIFIER);
    unmarshalling = VerifyResultInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteUint32(100);
    unmarshalling = VerifyResultInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
}
/**
 * @tc.name: AppDomainVerifyUrlUtilTest001
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanTest, AppDomainVerifyUrlUtilTest001, TestSize.Level0)
{
    ASSERT_TRUE(UrlUtil::IsValidAppDomainVerifyHost("https://" + HOST));
    ASSERT_TRUE(UrlUtil::IsValidUrl("https://" + HOST));
    ASSERT_TRUE(UrlUtil::GetScheme(HOST) == "");
}

/**
 * @tc.name: AppDomainVerifyCommonUtilTest001
 * @tc.desc: CommonUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanTest, AppDomainVerifyCommonUtilTest001, TestSize.Level0)
{
    ASSERT_TRUE(MaskStr("").empty());
    std::string ONE_TEST = "a";
    ASSERT_TRUE(MaskStr(ONE_TEST) == ONE_TEST);
    std::string NORMAL_TEST = "https://www.openharmony.cn/res";
    std::string NORMAL_TEST_EXPECT = "h**p**/**w**o**n**r**n**c**r**";
    ASSERT_TRUE(MaskStr(NORMAL_TEST) == NORMAL_TEST_EXPECT);
}

}
