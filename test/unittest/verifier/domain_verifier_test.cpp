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
#include "mock_constant.h"
#define private public
#define protected public
#include "domain_verifier.h"
#undef private
#undef protected
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class DomainVerifierTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DomainVerifierTest::SetUpTestCase(void)
{
}

void DomainVerifierTest::TearDownTestCase(void)
{
}

void DomainVerifierTest::SetUp(void)
{
}

void DomainVerifierTest::TearDown(void)
{
}
/**
 * @tc.name: DomainVerifierTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTest, DomainVerifierTest001, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    std::string assetJsonsStr = "";
    ASSERT_TRUE(DomainVerifier::VerifyHost(OHOS::NetStack::HttpClient::ResponseCode::OK, assetJsonsStr,
                    appVerifyBaseInfo) == InnerVerifyStatus::STATE_FAIL);
}
/**
 * @tc.name: DomainVerifierTest002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTest, DomainVerifierTest002, TestSize.Level0)
{
    ASSERT_TRUE(InnerVerifyStatus::FAILURE_REDIRECT ==
        DomainVerifier::GetVerifyStatusFromHttpError(OHOS::NetStack::HttpClient::ResponseCode::MOVED_PERM));
    ASSERT_TRUE(InnerVerifyStatus::FAILURE_CLIENT_ERROR ==
        DomainVerifier::GetVerifyStatusFromHttpError(OHOS::NetStack::HttpClient::ResponseCode::UNAUTHORIZED));
    ASSERT_TRUE(InnerVerifyStatus::FAILURE_REJECTED_BY_SERVER ==
        DomainVerifier::GetVerifyStatusFromHttpError(OHOS::NetStack::HttpClient::ResponseCode::INTERNAL_ERROR));
    ASSERT_TRUE(InnerVerifyStatus::FAILURE_HTTP_UNKNOWN ==
        DomainVerifier::GetVerifyStatusFromHttpError(OHOS::NetStack::HttpClient::ResponseCode::PARTIAL));
}
/**
 * @tc.name: DomainVerifierTest003
 * @tc.desc: VerifyHost test.
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTest, DomainVerifierTest003, TestSize.Level0)
{
    AssetJsonObj assetJsonObj;
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_FAIL == DomainVerifier::VerifyHostWithBundleName(assetJsonObj, appVerifyBaseInfo));
    appVerifyBaseInfo.fingerprint = FINGERPRINT;
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_FAIL == DomainVerifier::VerifyHostWithBundleName(assetJsonObj, appVerifyBaseInfo));
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfo);
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfo.fingerprint = FINGERPRINT;
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfo);
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_SUCCESS == DomainVerifier::VerifyHostWithBundleName(assetJsonObj, appVerifyBaseInfo));
}
/**
 * @tc.name: DomainVerifierTest004
 * @tc.desc: VerifyHost test.
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTest, DomainVerifierTest004, TestSize.Level0)
{
    AssetJsonObj assetJsonObj;
    AppVerifyBaseInfo appVerifyBaseInfo;
    AppVerifyBaseInfo appVerifyBaseInfoInApp;

    // appidentifier null
    appVerifyBaseInfo.appIdentifier = "";
    ASSERT_TRUE(
        InnerVerifyStatus::UNKNOWN == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    appVerifyBaseInfo.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    // apps empty
    ASSERT_TRUE(
        InnerVerifyStatus::UNKNOWN == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    //  1 app empty appIdentifier
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::UNKNOWN == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 1 app appIdentifier not null and not equal
    appVerifyBaseInfo.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfoInApp.appIdentifier = FINGERPRINT;
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::UNKNOWN == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));
}
/**
 * @tc.name: DomainVerifierTest005
 * @tc.desc: VerifyHost test.
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTest, DomainVerifierTest005, TestSize.Level0)
{
    AssetJsonObj assetJsonObj;
    AppVerifyBaseInfo appVerifyBaseInfo;
    AppVerifyBaseInfo appVerifyBaseInfoInApp;

    //  1.1 app empty appIdentifier
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::UNKNOWN == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 1.2 app appIdentifier not null and not equal
    appVerifyBaseInfo.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfoInApp.appIdentifier = FINGERPRINT;
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::UNKNOWN == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 1.3 app appIdentifier not null and not equal-> empty bundleName
    appVerifyBaseInfoInApp.appIdentifier = FINGERPRINT;
    appVerifyBaseInfoInApp.bundleName = "";
    ASSERT_TRUE(
        InnerVerifyStatus::UNKNOWN == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 1.4 app appIdentifier not null and not equal-> not empty bundleName, not equal
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfoInApp.bundleName = "test";
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::UNKNOWN == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 1.5 app appIdentifier not null and not equal-> not empty bundleName,  equal
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfoInApp.bundleName = BUNDLE_NAME;
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_FAIL == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));
}
/**
 * @tc.name: DomainVerifierTest006
 * @tc.desc: VerifyHost test.
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTest, DomainVerifierTest006, TestSize.Level0)
{
    AssetJsonObj assetJsonObj;
    AppVerifyBaseInfo appVerifyBaseInfo;
    AppVerifyBaseInfo appVerifyBaseInfoInApp;
    // appId equal
    appVerifyBaseInfo.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfoInApp.appIdentifier = APP_IDENTIFIER;

    // 2.1 equal-> base bundle null, inapp bundle null, not equal
    appVerifyBaseInfo.bundleName = "";
    appVerifyBaseInfoInApp.bundleName = "";
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_SUCCESS == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 2.2 equal-> base bundle not null, inapp bundle null, not equal
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfoInApp.bundleName = "";
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_SUCCESS == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 2.3 equal-> base bundle not null, inapp bundle not null, not equal
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfoInApp.bundleName = "test";
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_FAIL == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 2.4 equal-> base bundle not null, inapp bundle not null, equal
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfoInApp.bundleName = BUNDLE_NAME;
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_SUCCESS == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));
}
/**
 * @tc.name: DomainVerifierTest007
 * @tc.desc: VerifyHost test.
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTest, DomainVerifierTest007, TestSize.Level0)
{
    AssetJsonObj assetJsonObj;
    AppVerifyBaseInfo appVerifyBaseInfo;
    AppVerifyBaseInfo appVerifyBaseInfoInApp;
    appVerifyBaseInfo.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfoInApp.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfoInApp.bundleName = BUNDLE_NAME;

    // 2.5 equal-> base fingerprint null, inapp fingerprint null, not equal
    appVerifyBaseInfo.fingerprint = "";
    appVerifyBaseInfoInApp.fingerprint = "";
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_SUCCESS == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 2.6 equal-> base fingerprint not null, inapp fingerprint null, not equal
    appVerifyBaseInfoInApp.fingerprint = FINGERPRINT;
    appVerifyBaseInfo.fingerprint = "";
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_SUCCESS == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 2.7 equal-> base fingerprint not null, inapp fingerprint not null, not equal
    appVerifyBaseInfoInApp.fingerprint = FINGERPRINT;
    appVerifyBaseInfo.fingerprint = "test";
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_FAIL == DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));

    // 2.8 equal-> base fingerprint not null, inapp fingerprint not null, equal
    appVerifyBaseInfoInApp.fingerprint = FINGERPRINT;
    appVerifyBaseInfo.fingerprint = FINGERPRINT;
    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(
        InnerVerifyStatus::STATE_SUCCESS == 
            DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));
}

/**
 * @tc.name: DomainVerifierTest008
 * @tc.desc: VerifyHost test.
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTest, DomainVerifierTest008, TestSize.Level0)
{
    AssetJsonObj assetJsonObj;
    AppVerifyBaseInfo appVerifyBaseInfo;
    AppVerifyBaseInfo appVerifyBaseInfoInApp;

    // final
    appVerifyBaseInfo.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfoInApp.appIdentifier = APP_IDENTIFIER;

    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfoInApp.bundleName = BUNDLE_NAME;

    appVerifyBaseInfo.fingerprint = FINGERPRINT;
    appVerifyBaseInfoInApp.fingerprint = FINGERPRINT;

    assetJsonObj.applinking.apps.clear();
    assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfoInApp);
    ASSERT_TRUE(InnerVerifyStatus::STATE_SUCCESS ==
        DomainVerifier::VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo));
}
}