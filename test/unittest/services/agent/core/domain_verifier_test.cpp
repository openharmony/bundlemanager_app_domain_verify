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

}