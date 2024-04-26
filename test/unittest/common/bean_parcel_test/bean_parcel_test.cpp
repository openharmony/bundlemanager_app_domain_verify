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
#include "parcel.h"
#define private public
#define protected public
#include "skill_uri.h"
#include "app_verify_base_info.h"
#include "bundle_verify_status_info.h"
#include "domain_url_util.h"
#undef private
#undef protected
#include "app_domain_verify_hilog.h"

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
const std::string APP_IDENTIFIER = "appIdentifier";
const std::string BUNDLE_NAME = "com.openHarmony.test";
const std::string FINGERPRINT = "fingerprint";
const std::string HOST = "test.openharmony.com";
class AppDomainVerifyBeanParcelTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};
void AppDomainVerifyBeanParcelTest::SetUpTestCase(void)
{
}

void AppDomainVerifyBeanParcelTest::TearDownTestCase(void)
{
}

void AppDomainVerifyBeanParcelTest::SetUp(void)
{
}

void AppDomainVerifyBeanParcelTest::TearDown(void)
{
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest001
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest001, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    SkillUri skillUri;
    skillUri.scheme = "scheme";
    skillUri.host = "host";
    skillUri.port = "port";
    skillUri.path = "path";
    skillUri.pathStartWith = "pathStartWith";
    skillUri.pathRegex = "pathRegex";
    skillUri.type = "type";
    Parcel parcel;
    MockWriteString(false, 0);
    ASSERT_FALSE(skillUri.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 1);
    ASSERT_FALSE(skillUri.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 2);
    ASSERT_FALSE(skillUri.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 3);
    ASSERT_FALSE(skillUri.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 4);
    ASSERT_FALSE(skillUri.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 5);
    ASSERT_FALSE(skillUri.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 6);
    ASSERT_FALSE(skillUri.Marshalling(parcel));
    ResetParcelState();
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001 end");
    printf("AppDomainVerifyBeanParcelTest001 end\n");
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest002
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest002, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest002");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    SkillUri skillUri;
    skillUri.scheme = "scheme";
    skillUri.host = "host";
    skillUri.port = "port";
    skillUri.path = "path";
    skillUri.pathStartWith = "pathStartWith";
    skillUri.pathRegex = "pathRegex";
    skillUri.type = "type";
    Parcel parcel;
    ASSERT_TRUE(skillUri.Marshalling(parcel));

    SkillUri skillUriOut;

    MockReadString(false, 0);
    ASSERT_TRUE(skillUriOut.Unmarshalling(parcel) == nullptr);
    ResetParcelState();

    MockReadString(false, 1);
    ASSERT_TRUE(skillUriOut.Unmarshalling(parcel) == nullptr);
    ResetParcelState();

    MockReadString(false, 2);
    ASSERT_TRUE(skillUriOut.Unmarshalling(parcel) == nullptr);
    ResetParcelState();

    MockReadString(false, 3);
    ASSERT_TRUE(skillUriOut.Unmarshalling(parcel) == nullptr);
    ResetParcelState();

    MockReadString(false, 4);
    ASSERT_TRUE(skillUriOut.Unmarshalling(parcel) == nullptr);
    ResetParcelState();

    MockReadString(false, 5);
    ASSERT_TRUE(skillUriOut.Unmarshalling(parcel) == nullptr);
    ResetParcelState();

    MockReadString(false, 6);
    ASSERT_TRUE(skillUriOut.Unmarshalling(parcel) == nullptr);
    ResetParcelState();
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest002 end");
    printf("AppDomainVerifyBeanParcelTest001 end\n");
}


/**
 * @tc.name: AppDomainVerifyBeanParcelTest003
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest003, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.appIdentifier = "appIdentifier";
    appVerifyBaseInfo.bundleName = "bundleName";
    appVerifyBaseInfo.fingerprint = "fingerprint";
    Parcel parcel;
    MockWriteString(false, 0);
    ASSERT_FALSE(appVerifyBaseInfo.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 1);
    ASSERT_FALSE(appVerifyBaseInfo.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 2);
    ASSERT_FALSE(appVerifyBaseInfo.Marshalling(parcel));
    ResetParcelState();

    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001 end");
    printf("AppDomainVerifyBeanParcelTest001 end\n");
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest004
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest004, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest002");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.appIdentifier = "appIdentifier";
    appVerifyBaseInfo.bundleName = "bundleName";
    appVerifyBaseInfo.fingerprint = "fingerprint";
    Parcel parcel;
    ASSERT_TRUE(appVerifyBaseInfo.Marshalling(parcel));


    AppVerifyBaseInfo appVerifyBaseInfoOut(appVerifyBaseInfo);
    MockReadString(false, 0);
    ASSERT_TRUE(appVerifyBaseInfoOut.Unmarshalling(parcel) == nullptr);
    ResetParcelState();

    MockReadString(false, 1);
    ASSERT_TRUE(appVerifyBaseInfoOut.Unmarshalling(parcel) == nullptr);
    ResetParcelState();

    MockReadString(false, 2);
    ASSERT_TRUE(appVerifyBaseInfoOut.Unmarshalling(parcel) == nullptr);
    ResetParcelState();

    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest002 end");
    printf("AppDomainVerifyBeanParcelTest001 end\n");
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest005
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest005, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.insert_or_assign(BUNDLE_NAME, verifyResultInfo);

    Parcel parcel;
    MockWriteUint32(false);
    ASSERT_FALSE(bundleVerifyStatusInfo.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 0);
    ASSERT_FALSE(bundleVerifyStatusInfo.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 1);
    ASSERT_FALSE(bundleVerifyStatusInfo.Marshalling(parcel));
    ResetParcelState();

    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001 end");
    printf("AppDomainVerifyBeanParcelTest001 end\n");
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest006
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest006, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.insert_or_assign(BUNDLE_NAME, verifyResultInfo);

    BundleVerifyStatusInfo bundleVerifyStatusInfoOut;
    Parcel parcel;
    MockReadUint32(false);
    ASSERT_FALSE(bundleVerifyStatusInfoOut.Unmarshalling(parcel));
    ResetParcelState();

    MockReadString(false, 0);
    ASSERT_FALSE(bundleVerifyStatusInfoOut.Unmarshalling(parcel));
    ResetParcelState();

    MockReadString(false, 1);
    ASSERT_FALSE(bundleVerifyStatusInfoOut.Unmarshalling(parcel));
    ResetParcelState();

    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001 end");
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest007
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest007, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.insert_or_assign(BUNDLE_NAME, verifyResultInfo);

    std::unordered_map<std::string, InnerVerifyStatus> bundleVerifyStatusInfoMap;
    bundleVerifyStatusInfoMap.insert_or_assign(BUNDLE_NAME, InnerVerifyStatus::STATE_SUCCESS);
    Parcel parcel;
    MockWriteUint32(false);
    ASSERT_FALSE(bundleVerifyStatusInfo.WriteHostVerifyStatusMap(bundleVerifyStatusInfoMap, parcel));
    ResetParcelState();

    MockWriteString(false, 0);
    ASSERT_FALSE(bundleVerifyStatusInfo.WriteHostVerifyStatusMap(bundleVerifyStatusInfoMap, parcel));
    ResetParcelState();

    MockWriteInt32(false);
    ASSERT_FALSE(bundleVerifyStatusInfo.WriteHostVerifyStatusMap(bundleVerifyStatusInfoMap, parcel));
    ResetParcelState();

    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001 end");
    printf("AppDomainVerifyBeanParcelTest001 end\n");
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest008
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest008, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.insert_or_assign(BUNDLE_NAME, verifyResultInfo);

    std::unordered_map<std::string, InnerVerifyStatus> bundleVerifyStatusInfoMap;
    bundleVerifyStatusInfoMap.insert_or_assign(BUNDLE_NAME, InnerVerifyStatus::STATE_SUCCESS);
    Parcel parcel;
    MockReadUint32(false);
    ASSERT_FALSE(bundleVerifyStatusInfo.ReadHostVerifyStatusMap(bundleVerifyStatusInfoMap, parcel));
    ResetParcelState();

    MockReadString(false, 0);
    ASSERT_FALSE(bundleVerifyStatusInfo.ReadHostVerifyStatusMap(bundleVerifyStatusInfoMap, parcel));
    ResetParcelState();

    MockReadInt32(false);
    ASSERT_FALSE(bundleVerifyStatusInfo.ReadHostVerifyStatusMap(bundleVerifyStatusInfoMap, parcel));
    ResetParcelState();

    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001 end");
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest009
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest009, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = APP_IDENTIFIER;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);

    Parcel parcel;
    MockWriteString(false, 0);
    ASSERT_FALSE(verifyResultInfo.Marshalling(parcel));
    ResetParcelState();

    MockWriteUint32(false);
    ASSERT_FALSE(verifyResultInfo.Marshalling(parcel));
    ResetParcelState();

    MockWriteString(false, 1);
    ASSERT_FALSE(verifyResultInfo.Marshalling(parcel));
    ResetParcelState();

    MockWriteInt32(false);
    ASSERT_FALSE(verifyResultInfo.Marshalling(parcel));
    ResetParcelState();

    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001 end");
    printf("AppDomainVerifyBeanParcelTest001 end\n");
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest0010
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest0010, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest002");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = APP_IDENTIFIER;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);

    Parcel parcel;
    MockReadString(false, 0);
    ASSERT_FALSE(verifyResultInfo.Unmarshalling(parcel));
    ResetParcelState();

    MockReadUint32(false);
    ASSERT_FALSE(verifyResultInfo.Unmarshalling(parcel));
    ResetParcelState();

    MockReadString(false, 1);
    ASSERT_FALSE(verifyResultInfo.Unmarshalling(parcel));
    ResetParcelState();

    MockReadInt32(false);
    ASSERT_FALSE(verifyResultInfo.Unmarshalling(parcel));
    ResetParcelState();

    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest002 end");
    printf("AppDomainVerifyBeanParcelTest001 end\n");
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest011
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest011, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001");
    printf("AppDomainVerifyBeanParcelTest001 \n");
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = APP_IDENTIFIER;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://", InnerVerifyStatus::STATE_FAIL);
    auto jsonObj = VerifyResultInfo::VerifyResultInfoToJson(verifyResultInfo);

    auto verifyResultInfoOut = VerifyResultInfo::JsonToVerifyResultInfo(jsonObj);
    ASSERT_TRUE(verifyResultInfoOut.appIdentifier == APP_IDENTIFIER);
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest001 end");
    printf("AppDomainVerifyBeanParcelTest001 end\n");
}

/**
 * @tc.name: AppDomainVerifyBeanParcelTest0012
 * @tc.desc: UrlUtil test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyBeanParcelTest, AppDomainVerifyBeanParcelTest0012, TestSize.Level0)
{
    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest002");
    printf("AppDomainVerifyBeanParcelTest0012 \n");
    json jsonObj;
    VerifyResultInfo verifyResultInfoOut;


    jsonObj = json::parse(R"({"appIdentifier":5,"hostVerifyStatusMap":{"key":1}})");
    verifyResultInfoOut = VerifyResultInfo::JsonToVerifyResultInfo(jsonObj);
    ASSERT_TRUE(verifyResultInfoOut.appIdentifier.empty());

    jsonObj = json::parse(R"({"appIdentifier":"appIdentifier","hostVerifyStatusMap":5})");
    verifyResultInfoOut = VerifyResultInfo::JsonToVerifyResultInfo(jsonObj);
    ASSERT_TRUE(verifyResultInfoOut.appIdentifier == "appIdentifier");

    jsonObj = json::parse(R"({"appIdentifier":"appIdentifier","hostVerifyStatusMap":{"key":1,"key2":2}})");
    verifyResultInfoOut = VerifyResultInfo::JsonToVerifyResultInfo(jsonObj);
    ASSERT_TRUE(verifyResultInfoOut.appIdentifier == "appIdentifier");

    jsonObj = json::parse(R"({"appIdentifier":"appIdentifier","hostVerifyStatusMap":{"key":1}})");
    verifyResultInfoOut = VerifyResultInfo::JsonToVerifyResultInfo(jsonObj);
    ASSERT_TRUE(verifyResultInfoOut.appIdentifier == "appIdentifier");

    jsonObj = json::parse(R"({"appIdentifier":"appIdentifier","hostVerifyStatusMap":{}})");
    verifyResultInfoOut = VerifyResultInfo::JsonToVerifyResultInfo(jsonObj);
    ASSERT_TRUE(verifyResultInfoOut.appIdentifier == "appIdentifier");

    APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "AppDomainVerifyBeanParcelTest002 end");
    printf("AppDomainVerifyBeanParcelTest0012 end\n");
}
}
