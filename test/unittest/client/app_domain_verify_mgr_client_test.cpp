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
int MgrInvokeOK(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "%s call end", __func__);
    g_mgrInvokeOK = true;
    return 0;
}

int MgrInvokeFail(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
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
}

void AppDomainVerifyMgrClientTest::TearDown(void)
{
}

/**
 * @tc.name: AppDomainVerifyMgrClientTest001
 * @tc.desc: ClearDomainVerifyStatus ok test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrClientTest, AppDomainVerifyMgrClientTest001, TestSize.Level0)
{
    AppDomainVerifyMgrClient::GetInstance()->ClearDomainVerifyStatus(APP_IDENTIFIER, BUNDLE_NAME);
    DomainVerifyStatus domainVerificationState;
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryDomainVerifyStatus(BUNDLE_NAME,
        domainVerificationState);
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
    auto filterRes = AppDomainVerifyMgrClient::GetInstance()->FilterAbilities(want, originAbilityInfos,
        filtedAbilityInfos);
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
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.insert_or_assign(BUNDLE_NAME, verifyResultInfo);
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryAllDomainVerifyStatus(bundleVerifyStatusInfo);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.empty());
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
    EXPECT_CALL(*mgrStubMock_, SendRequest(_, _, _, _)).Times(1).WillOnce(::testing::Invoke(MgrInvokeOK));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = sptr<AppDomainVerifyMgrServiceProxy>::MakeSptr(
        mgrStubMock_.get());
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    AppDomainVerifyMgrClient::GetInstance()->VerifyDomain(APP_IDENTIFIER, BUNDLE_NAME, FINGERPRINT, skillUris);
    ASSERT_TRUE(g_mgrInvokeOK);
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
    EXPECT_CALL(*mgrStubMock_, SendRequest(_, _, _, _)).Times(1).WillOnce(::testing::Invoke(MgrInvokeOK));
    AppDomainVerifyMgrClient::appDomainVerifyMgrServiceProxy_ = sptr<AppDomainVerifyMgrServiceProxy>::MakeSptr(
        mgrStubMock_.get());
    std::string bundleName = BUNDLE_NAME;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);
    AppDomainVerifyMgrClient::GetInstance()->SaveDomainVerifyStatus(BUNDLE_NAME, verifyResultInfo);
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

}
