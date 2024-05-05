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
#include <memory>
#include <string>
#include "mock_verify_agent.h"
#include "app_domain_verify_agent_service_proxy.h"
#include "refbase.h"
#include "mock_constant.h"
#include "mock_verify_mgr.h"
#include "app_domain_verify_mgr_interface_code.h"
#define private public
#define protected public
#include "app_domain_verify_mgr_client.h"
#include "app_domain_verify_agent_client.h"
#include "app_domain_verify_mgr_service.h"
#undef private
#undef protected

namespace OHOS::AppDomainVerify {
using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using namespace testing;
using namespace testing::ext;
class AppDomainVerifyMgrModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};
auto appDomainVerifyMgrService_ = std::make_shared<AppDomainVerifyMgrService>();
auto appDomainVerifyAgentStubMock_ = std::make_shared<AppDomainVerifyAgentRemoteStubMock>();
int InvokeSingleVerifyOK(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "%s call end", __func__);
    std::string bundleName = BUNDLE_NAME;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);
    appDomainVerifyMgrService_->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
    return 0;
}

int InvokeSingleVerifyFail(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "%s call end", __func__);
    std::string bundleName = BUNDLE_NAME;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_FAIL);
    appDomainVerifyMgrService_->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
    return UNKNOWN_ERROR;
}

void AppDomainVerifyMgrModuleTest::SetUpTestCase(void)
{
}

void AppDomainVerifyMgrModuleTest::TearDownTestCase(void)
{
    appDomainVerifyMgrService_.reset();
    appDomainVerifyAgentStubMock_.reset();
}

void AppDomainVerifyMgrModuleTest::SetUp(void)
{
    AppDomainVerifyAgentClient::staticDestoryMonitor_.destoryed_ = true;
}

void AppDomainVerifyMgrModuleTest::TearDown(void)
{
}
/**
 * @tc.name: AppDomainVerifyMgrModuleTest001
 * @tc.desc: VerifyDomain ok test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest001, TestSize.Level0)
{
    EXPECT_CALL(*appDomainVerifyAgentStubMock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(::testing::Invoke(InvokeSingleVerifyOK));
    AppDomainVerifyAgentClient::agentServiceProxy_ = sptr<AppDomainVerifyAgentServiceProxy>::MakeSptr(
        appDomainVerifyAgentStubMock_.get());
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    appDomainVerifyMgrService_->VerifyDomain(APP_IDENTIFIER, BUNDLE_NAME, FINGERPRINT, skillUris);
    DomainVerifyStatus domainVerificationState;
    auto queryRes = appDomainVerifyMgrService_->QueryDomainVerifyStatus(BUNDLE_NAME, domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_VERIFIED);
}

/**
 * @tc.name: AppDomainVerifyMgrModuleTest002
 * @tc.desc: VerifyDomain fail test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest002, TestSize.Level0)
{
    EXPECT_CALL(*appDomainVerifyAgentStubMock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(::testing::Invoke(InvokeSingleVerifyFail));
    AppDomainVerifyAgentClient::agentServiceProxy_ = sptr<AppDomainVerifyAgentServiceProxy>::MakeSptr(
        appDomainVerifyAgentStubMock_.get());

    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    appDomainVerifyMgrService_->VerifyDomain(APP_IDENTIFIER, BUNDLE_NAME, FINGERPRINT, skillUris);
    DomainVerifyStatus domainVerificationState;
    auto queryRes = appDomainVerifyMgrService_->QueryDomainVerifyStatus(BUNDLE_NAME, domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}

/**
 * @tc.name: AppDomainVerifyMgrModuleTest003
 * @tc.desc: FilterAbilities ok test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest003, TestSize.Level0)
{
    EXPECT_CALL(*appDomainVerifyAgentStubMock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(::testing::Invoke(InvokeSingleVerifyOK));
    AppDomainVerifyAgentClient::agentServiceProxy_ = sptr<AppDomainVerifyAgentServiceProxy>::MakeSptr(
        appDomainVerifyAgentStubMock_.get());

    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    appDomainVerifyMgrService_->VerifyDomain(APP_IDENTIFIER, BUNDLE_NAME, FINGERPRINT, skillUris);
    OHOS::AAFwk::Want want;
    want.SetUri("https://" + HOST);
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME;
    std::vector<OHOS::AppExecFwk::AbilityInfo> originAbilityInfos;
    originAbilityInfos.emplace_back(abilityInfo);
    std::vector<OHOS::AppExecFwk::AbilityInfo> filtedAbilityInfos;
    auto filterRes = appDomainVerifyMgrService_->FilterAbilities(want, originAbilityInfos, filtedAbilityInfos);
    ASSERT_TRUE(filterRes);
    ASSERT_TRUE(!filtedAbilityInfos.empty());
    ASSERT_TRUE(filtedAbilityInfos[0].bundleName == BUNDLE_NAME);
}

/**
 * @tc.name: AppDomainVerifyMgrModuleTest004
 * @tc.desc: FilterAbilities fail test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest004, TestSize.Level0)
{
    EXPECT_CALL(*appDomainVerifyAgentStubMock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(::testing::Invoke(InvokeSingleVerifyFail));
    AppDomainVerifyAgentClient::agentServiceProxy_ = sptr<AppDomainVerifyAgentServiceProxy>::MakeSptr(
        appDomainVerifyAgentStubMock_.get());
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    appDomainVerifyMgrService_->VerifyDomain(APP_IDENTIFIER, BUNDLE_NAME, FINGERPRINT, skillUris);

    OHOS::AAFwk::Want want;
    want.SetUri("https://" + HOST);
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME;
    std::vector<OHOS::AppExecFwk::AbilityInfo> originAbilityInfos;
    originAbilityInfos.emplace_back(abilityInfo);

    std::vector<OHOS::AppExecFwk::AbilityInfo> filtedAbilityInfos;
    auto filterRes = appDomainVerifyMgrService_->FilterAbilities(want, originAbilityInfos, filtedAbilityInfos);
    ASSERT_TRUE(filterRes);
    ASSERT_TRUE(filtedAbilityInfos.empty());
}

/**
 * @tc.name: AppDomainVerifyMgrModuleTest005
 * @tc.desc: FilterAbilities fail test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest005, TestSize.Level0)
{
    OHOS::AAFwk::Want want;
    want.SetElementName(BUNDLE_NAME, "MainAbility");
    want.SetUri("https://" + HOST);
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME;
    std::vector<OHOS::AppExecFwk::AbilityInfo> originAbilityInfos;
    originAbilityInfos.emplace_back(abilityInfo);

    std::vector<OHOS::AppExecFwk::AbilityInfo> filtedAbilityInfos;
    auto filterRes = appDomainVerifyMgrService_->FilterAbilities(want, originAbilityInfos, filtedAbilityInfos);
    ASSERT_FALSE(filterRes);
    ASSERT_TRUE(filtedAbilityInfos.empty());
}

/**
 * @tc.name: AppDomainVerifyMgrModuleTest006
 * @tc.desc: QueryDomainVerifyStatus test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest006, TestSize.Level0)
{
    DomainVerifyStatus domainVerificationState;
    auto queryRes = appDomainVerifyMgrService_->QueryDomainVerifyStatus(BUNDLE_NAME, domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}


/**
 * @tc.name: AppDomainVerifyMgrModuleTest007
 * @tc.desc: stub query test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest007, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(AppDomainVerifyMgrServiceStub::GetDescriptor());
    data.WriteString(BUNDLE_NAME);
    auto res = appDomainVerifyMgrStubMock->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::QUERY_VERIFY_STATUS, data,
        reply, option);
    ASSERT_TRUE(res == ERR_OK);
}

/**
 * @tc.name: AppDomainVerifyMgrModuleTest08
 * @tc.desc: stub domain verify test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest08, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(AppDomainVerifyMgrServiceStub::GetDescriptor());
    data.WriteString(APP_IDENTIFIER);
    data.WriteString(BUNDLE_NAME);
    data.WriteString(FINGERPRINT);
    data.WriteUint32(1);
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    data.WriteParcelable(&skillUri);
    auto res = appDomainVerifyMgrStubMock->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN, data, reply,
        option);
    ASSERT_TRUE(res == ERR_OK);
}

/**
 * @tc.name: AppDomainVerifyMgrModuleTest009
 * @tc.desc: stub clear test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest009, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(AppDomainVerifyMgrServiceStub::GetDescriptor());
    data.WriteString(APP_IDENTIFIER);
    data.WriteString(BUNDLE_NAME);

    auto res = appDomainVerifyMgrStubMock->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::CLEAR_DOMAIN_VERIFY_RESULT,
        data, reply, option);
    ASSERT_TRUE(res == ERR_OK);
    ASSERT_TRUE(reply.ReadBool());
}
/**
 * @tc.name: AppDomainVerifyMgrModuleTest010
 * @tc.desc: stub filter test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest010, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(AppDomainVerifyMgrServiceStub::GetDescriptor());
    OHOS::AAFwk::Want want;
    want.SetUri("https://" + HOST);
    data.WriteParcelable(&want);

    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME;
    data.WriteUint32(1);
    data.WriteParcelable(&abilityInfo);

    auto res = appDomainVerifyMgrStubMock->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data,
        reply, option);
    ASSERT_TRUE(res == ERR_OK);
    ASSERT_TRUE(reply.ReadBool());
    ASSERT_TRUE(reply.ReadInt32() == 0);
}

/**
 * @tc.name: AppDomainVerifyMgrModuleTest011
 * @tc.desc: stub query all test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest011, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(AppDomainVerifyMgrServiceStub::GetDescriptor());

    auto res = appDomainVerifyMgrStubMock->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::QUERY_ALL_VERIFY_STATUS,
        data, reply, option);
    ASSERT_TRUE(res == ERR_OK);
    ASSERT_TRUE(reply.ReadBool());
}

/**
 * @tc.name: AppDomainVerifyMgrModuleTest012
 * @tc.desc: stub save test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyMgrModuleTest, AppDomainVerifyMgrModuleTest012, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(AppDomainVerifyMgrServiceStub::GetDescriptor());
    data.WriteString(BUNDLE_NAME);
    VerifyResultInfo verifyResultInfo;
    data.WriteParcelable(&verifyResultInfo);

    auto res = appDomainVerifyMgrStubMock->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::SAVE_VERIFY_STATUS, data,
        reply, option);
    ASSERT_TRUE(res == ERR_OK);
    ASSERT_TRUE(reply.ReadBool());
}

}
