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
#include <string>
#include "mock_constant.h"
#include "mock_verify_mgr.h"
#define private public
#define protected public
#include "app_domain_verify_agent_client.h"
#include "app_domain_verify_mgr_client.h"
#undef private
#undef protected
#include "convert_callback_stub.h"
#include "mock_access_token.h"

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class AppDomainVerifyMgrClientTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

bool g_mgrInvokeOK = false;
int MgrInvokeOK(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "call end");
    reply.WriteBool(true);
    g_mgrInvokeOK = true;
    return 0;
}

int MgrInvokeFail(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    g_mgrInvokeOK = false;
    return UNKNOWN_ERROR;
}

void AppDomainVerifyMgrClientTest::SetUpTestCase(void)
{
}

void AppDomainVerifyMgrClientTest::TearDownTestCase(void)
{
}

void AppDomainVerifyMgrClientTest::SetUp(void)
{
    MockAccessToken::mockSA();
}

void AppDomainVerifyMgrClientTest::TearDown(void)
{
}
class CallBack : public ConvertCallbackStub {
public:
    void OnConvert(int resCode, AAFwk::Want& want) override
    {
    }
};
/**
 * @tc.name: AppDomainVerifyMgrClientTest001
 * @tc.desc: ClearDomainVerifyStatus ok test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest001, TestSize.Level0)
{
    AppDomainVerifyMgrClient::GetInstance()->ClearDomainVerifyStatus(APP_IDENTIFIER, BUNDLE_NAME);
    DomainVerifyStatus domainVerificationState;
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryDomainVerifyStatus(
        BUNDLE_NAME, domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}

/**
 * @tc.name: AppDomainVerifyMgrClientTest002
 * @tc.desc: FilterAbilities test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest002, TestSize.Level0)
{
    OHOS::AAFwk::Want want;
    want.SetUri("https://" + HOST);
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME;
    std::vector<OHOS::AppExecFwk::AbilityInfo> originAbilityInfos;
    originAbilityInfos.emplace_back(abilityInfo);
    std::vector<OHOS::AppExecFwk::AbilityInfo> filtedAbilityInfos;
    auto filterRes = AppDomainVerifyMgrClient::GetInstance()->FilterAbilities(
        want, originAbilityInfos, filtedAbilityInfos);
    ASSERT_TRUE(filterRes);
    ASSERT_TRUE(filtedAbilityInfos.empty());
}

/**
 * @tc.name: AppDomainVerifyMgrClientTest003
 * @tc.desc: QueryAllDomainVerifyStatus test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest003, TestSize.Level0)
{
    AppDomainVerifyMgrClient::GetInstance()->ClearDomainVerifyStatus(APP_IDENTIFIER, BUNDLE_NAME);
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        "https://" + HOST, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.insert_or_assign(BUNDLE_NAME, verifyResultInfo);
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryAllDomainVerifyStatus(bundleVerifyStatusInfo);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.count(BUNDLE_NAME) == 0);
}

/**
 * @tc.name: AppDomainVerifyMgrClientTest004
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest004, TestSize.Level0)
{
    std::shared_ptr<AppDomainVerifyMgrRemoteStubMock> mgrStubMock_ =
        std::make_shared<AppDomainVerifyMgrRemoteStubMock>();
    EXPECT_CALL(*mgrStubMock_, SendRequest(_, _, _, _)).Times(2).WillOnce(::testing::Invoke(MgrInvokeOK));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = sptr<AppDomainVerifyMgrServiceProxy>::MakeSptr(
        mgrStubMock_.get());
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    AppDomainVerifyMgrClient::GetInstance()->VerifyDomain(APP_IDENTIFIER, BUNDLE_NAME, FINGERPRINT, skillUris);
    ASSERT_TRUE(g_mgrInvokeOK);
    AppDomainVerifyMgrClient::GetInstance()->ClearDomainVerifyStatus(APP_IDENTIFIER, BUNDLE_NAME);
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_.ForceSetRefPtr(nullptr);
}
/**
 * @tc.name: AppDomainVerifyMgrClientTest004
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest005, TestSize.Level0)
{
    std::shared_ptr<AppDomainVerifyMgrRemoteStubMock> mgrStubMock_ =
        std::make_shared<AppDomainVerifyMgrRemoteStubMock>();
    EXPECT_CALL(*mgrStubMock_, SendRequest(_, _, _, _)).Times(2).WillOnce(::testing::Invoke(MgrInvokeOK));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = sptr<AppDomainVerifyMgrServiceProxy>::MakeSptr(
        mgrStubMock_.get());
    std::string bundleName = BUNDLE_NAME;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        "https://" + HOST, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    AppDomainVerifyMgrClient::GetInstance()->SaveDomainVerifyStatus(BUNDLE_NAME, verifyResultInfo);
    ASSERT_TRUE(g_mgrInvokeOK);
    AppDomainVerifyMgrClient::GetInstance()->ClearDomainVerifyStatus(APP_IDENTIFIER, BUNDLE_NAME);
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_.ForceSetRefPtr(nullptr);
}
/**
 * @tc.name: AppDomainVerifyMgrClientTest006
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
#define MAX_PATH_LEN (99)
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest006, TestSize.Level0)
{
    std::shared_ptr<AppDomainVerifyMgrRemoteStubMock> mgrStubMock_ =
        std::make_shared<AppDomainVerifyMgrRemoteStubMock>();
    EXPECT_CALL(*mgrStubMock_, SendRequest(_, _, _, _)).WillRepeatedly(::testing::Invoke(MgrInvokeOK));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = sptr<AppDomainVerifyMgrServiceProxy>::MakeSptr(
        mgrStubMock_.get());

    ASSERT_TRUE(AppDomainVerifyMgrClient::GetInstance()->IsAtomicServiceUrl("https://www.openharmony.com/t1_-est"));

    ASSERT_FALSE(AppDomainVerifyMgrClient::GetInstance()->IsAtomicServiceUrl("https://www.openharmony.com"));

    ASSERT_FALSE(AppDomainVerifyMgrClient::GetInstance()->IsAtomicServiceUrl("https://www.openharmony.com/"));

    ASSERT_TRUE(AppDomainVerifyMgrClient::GetInstance()->IsAtomicServiceUrl("https://www.openharmony.com/t1_-est/"));

    ASSERT_FALSE(AppDomainVerifyMgrClient::GetInstance()->IsAtomicServiceUrl("www.openharmony.com/feitj"));

    ASSERT_FALSE(AppDomainVerifyMgrClient::GetInstance()->IsAtomicServiceUrl("test://www.openharmony.com/test"));

    ASSERT_FALSE(AppDomainVerifyMgrClient::GetInstance()->IsAtomicServiceUrl("https://www.openharmony.com/test*"));

    ASSERT_FALSE(AppDomainVerifyMgrClient::GetInstance()->IsAtomicServiceUrl("https://www.openharmony.com/test/test"));

    std::string longPath(MAX_PATH_LEN + 1, 'a');
    ASSERT_FALSE(
        AppDomainVerifyMgrClient::GetInstance()->IsAtomicServiceUrl("https://www.openharmony.com/" + longPath));

    std::string longPath1(MAX_PATH_LEN, 'a');
    ASSERT_TRUE(
        AppDomainVerifyMgrClient::GetInstance()->IsAtomicServiceUrl("https://www.openharmony.com/" + longPath1));

    ASSERT_TRUE(g_mgrInvokeOK);
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_.ForceSetRefPtr(nullptr);
}

/**
 * @tc.name: AppDomainVerifyMgrClientTest007
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest007, TestSize.Level0)
{
    std::shared_ptr<AppDomainVerifyMgrRemoteStubMock> mgrStubMock_ =
        std::make_shared<AppDomainVerifyMgrRemoteStubMock>();
    EXPECT_CALL(*mgrStubMock_, SendRequest(_, _, _, _)).Times(1).WillOnce(::testing::Invoke(MgrInvokeOK));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = sptr<AppDomainVerifyMgrServiceProxy>::MakeSptr(
        mgrStubMock_.get());

    OHOS::AAFwk::Want atomicWant;
    sptr<IConvertCallback> cb = new CallBack;
    AppDomainVerifyMgrClient::GetInstance()->ConvertToExplicitWant(atomicWant, cb);
    ASSERT_TRUE(g_mgrInvokeOK);
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_.ForceSetRefPtr(nullptr);
}

/**
 * @tc.name: AppDomainVerifyMgrClientTest008
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest008, TestSize.Level0)
{
    std::shared_ptr<AppDomainVerifyMgrRemoteStubMock> mgrStubMock_ =
        std::make_shared<AppDomainVerifyMgrRemoteStubMock>();
    EXPECT_CALL(*mgrStubMock_, SendRequest(_, _, _, _)).Times(1).WillOnce(::testing::Invoke(MgrInvokeOK));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = sptr<AppDomainVerifyMgrServiceProxy>::MakeSptr(
        mgrStubMock_.get());

    std::vector<std::string> urls;
    AppDomainVerifyMgrClient::GetInstance()->UpdateWhiteListUrls(urls);
    ASSERT_TRUE(g_mgrInvokeOK);
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_.ForceSetRefPtr(nullptr);
}

/**
 * @tc.name: AppDomainVerifyMgrClientTest009
 * @tc.desc: QueryAssociatedBundleNames test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest009, TestSize.Level0)
{
    std::shared_ptr<AppDomainVerifyMgrRemoteStubMock> mgrStubMock_ =
        std::make_shared<AppDomainVerifyMgrRemoteStubMock>();
    EXPECT_CALL(*mgrStubMock_, SendRequest(_, _, _, _)).Times(1).WillOnce(::testing::Invoke(MgrInvokeOK));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = sptr<AppDomainVerifyMgrServiceProxy>::MakeSptr(
        mgrStubMock_.get());

    std::vector<std::string> bundleNames;
    AppDomainVerifyMgrClient::GetInstance()->QueryAssociatedBundleNames("Domain", bundleNames);
    ASSERT_TRUE(g_mgrInvokeOK);
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_.ForceSetRefPtr(nullptr);
}

/**
 * @tc.name: AppDomainVerifyMgrClientTest010
 * @tc.desc: QueryAssociatedDomains test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest010, TestSize.Level0)
{
    std::shared_ptr<AppDomainVerifyMgrRemoteStubMock> mgrStubMock_ =
        std::make_shared<AppDomainVerifyMgrRemoteStubMock>();
    EXPECT_CALL(*mgrStubMock_, SendRequest(_, _, _, _)).Times(1).WillOnce(::testing::Invoke(MgrInvokeOK));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = sptr<AppDomainVerifyMgrServiceProxy>::MakeSptr(
        mgrStubMock_.get());

    std::vector<std::string> domains;
    AppDomainVerifyMgrClient::GetInstance()->QueryAssociatedDomains(BUNDLE_NAME, domains);
    ASSERT_TRUE(g_mgrInvokeOK);
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_.ForceSetRefPtr(nullptr);
}
/**
 * @tc.name: AppDomainVerifyMgrSaDeathRecipientTest001
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrSaDeathRecipientTest001, TestSize.Level0)
{
    std::unique_ptr<AppDomainVerifyMgrRemoteStubMock> impl = std::make_unique<AppDomainVerifyMgrRemoteStubMock>();
    sptr<AppDomainVerifyMgrRemoteStubMock> mgrStubMock_(impl.get());
    EXPECT_CALL(*mgrStubMock_, AsObject()).Times(1).WillOnce(::testing::Return(nullptr));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = mgrStubMock_;

    AppDomainVerifyMgrSaDeathRecipient AppDomainVerifyMgrSaDeathRecipient;
    AppDomainVerifyMgrSaDeathRecipient.OnRemoteDied(nullptr);
    ASSERT_TRUE(AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ == nullptr);
    mgrStubMock_.ForceSetRefPtr(nullptr);
}

/**
 * @tc.name: AppDomainVerifyMgrSaDeathRecipientTest002
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrSaDeathRecipientTest002, TestSize.Level0)
{
    std::unique_ptr<AppDomainVerifyMgrRemoteStubMock> impl = std::make_unique<AppDomainVerifyMgrRemoteStubMock>();
    sptr<AppDomainVerifyMgrRemoteStubMock> mgrStubMock_(impl.get());
    EXPECT_CALL(*mgrStubMock_, AsObject()).Times(1).WillOnce(::testing::Return(mgrStubMock_));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = mgrStubMock_;

    AppDomainVerifyMgrSaDeathRecipient AppDomainVerifyMgrSaDeathRecipient;
    AppDomainVerifyMgrSaDeathRecipient.OnRemoteDied(nullptr);
    ASSERT_TRUE(AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ == nullptr);
    mgrStubMock_.ForceSetRefPtr(nullptr);
}

/**
 * @tc.name: AppDomainVerifyMgrSaDeathRecipientTest003
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrSaDeathRecipientTest003, TestSize.Level0)
{
    AppDomainVerifyMgrSaDeathRecipient AppDomainVerifyMgrSaDeathRecipient;
    AppDomainVerifyMgrSaDeathRecipient.OnRemoteDied(nullptr);
    ASSERT_TRUE(AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ == nullptr);
}

/**
 * @tc.name: IsServiceAvailable_0100
 * @tc.desc: Test IsServiceAvailable.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, IsServiceAvailable_0100, TestSize.Level0)
{
    AppDomainVerifyMgrSaDeathRecipient AppDomainVerifyMgrSaDeathRecipient;
    AppDomainVerifyMgrSaDeathRecipient.OnRemoteDied(nullptr);
    auto res = AppDomainVerifyMgrClient::GetInstance()->IsServiceAvailable();
    EXPECT_NE(AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_, nullptr);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: ConnectService_0100
 * @tc.desc: Test ConnectService.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, ConnectService_0100, TestSize.Level0)
{
    AppDomainVerifyMgrSaDeathRecipient AppDomainVerifyMgrSaDeathRecipient;
    AppDomainVerifyMgrSaDeathRecipient.OnRemoteDied(nullptr);
    AppDomainVerifyMgrClient::GetInstance()->ConnectService();
    EXPECT_NE(AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_, nullptr);
}

/**
 * @tc.name: OnRemoteSaDied_0100
 * @tc.desc: Test OnRemoteSaDied.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, OnRemoteSaDied_0100, TestSize.Level0)
{
    AppDomainVerifyMgrClient::GetInstance()->ConnectService();
    AppDomainVerifyMgrClient::GetInstance()->OnRemoteSaDied(nullptr);
    EXPECT_EQ(AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_, nullptr);
}

/**
 * @tc.name: IsValidUrl_0100
 * @tc.desc: Test IsValidUrl.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, IsValidUrl_0100, TestSize.Level0)
{
    OHOS::Uri schemeWrongUri("schemeWrongUri");
    EXPECT_FALSE(AppDomainVerifyMgrClient::GetInstance()->IsValidUrl(schemeWrongUri));
    OHOS::Uri hostEmptyUri("https://");
    EXPECT_FALSE(AppDomainVerifyMgrClient::GetInstance()->IsValidUrl(hostEmptyUri));
    OHOS::Uri segmentsSizeWrongUri("https://www.openharmony.com/");
    EXPECT_FALSE(AppDomainVerifyMgrClient::GetInstance()->IsValidUrl(segmentsSizeWrongUri));
    OHOS::Uri segmentsWrongUri("https://www.openharmony.com/test*");
    EXPECT_FALSE(AppDomainVerifyMgrClient::GetInstance()->IsValidUrl(segmentsWrongUri));
    OHOS::Uri trueUri("https://www.openharmony.com/t1_-est/");
    EXPECT_TRUE(AppDomainVerifyMgrClient::GetInstance()->IsValidUrl(trueUri));
}

/**
 * @tc.name: IsValidPath_0100
 * @tc.desc: Test IsValidPath.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, IsValidPath_0100, TestSize.Level0)
{
    OHOS::Uri uri("https://www.openharmony.com/t1_-est/");
    std::vector<std::string> segments;
    uri.GetPathSegments(segments);
    EXPECT_TRUE(AppDomainVerifyMgrClient::GetInstance()->IsValidPath(segments[0]));
}
}
