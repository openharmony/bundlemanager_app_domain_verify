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
#include "mock_verify_agent.h"
#include "mock_constant.h"
#define private public
#define protected public
#include "app_domain_verify_agent_client.h"
#include "app_domain_verify_agent_service.h"
#include "app_domain_verify_mgr_client.h"
#undef private
#undef protected
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
namespace OHOS::AppDomainVerify {
using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using namespace testing;
using namespace testing::ext;

class AppDomainVerifyAgentClientTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

bool g_enterAgentInvokeOK = false;

int AgentInvokeOK(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "%s call end", __func__);
    g_enterAgentInvokeOK = true;
    return 0;
}

int AgentInvokeFail(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    g_enterAgentInvokeOK = false;
    return UNKNOWN_ERROR;
}

void AppDomainVerifyAgentClientTest::SetUpTestCase(void)
{
}

void AppDomainVerifyAgentClientTest::TearDownTestCase(void)
{
}

void AppDomainVerifyAgentClientTest::SetUp(void)
{
    AppDomainVerifyAgentClient::staticDestoryMonitor_.destoryed_ = true;
}

void AppDomainVerifyAgentClientTest::TearDown(void)
{
}

/**
 * @tc.name: AppDomainVerifyAgentClientTest001
 * @tc.desc: client SingleVerify ok test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentClientTest, AppDomainVerifyAgentClientTest001, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfo.fingerprint = FINGERPRINT;
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    AppDomainVerifyAgentClient::GetInstance()->SingleVerify(appVerifyBaseInfo, skillUris);
    DomainVerifyStatus domainVerificationState;
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryDomainVerifyStatus(BUNDLE_NAME,
        domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}

/**
 * @tc.name: AppDomainVerifyAgentClientTest003
 * @tc.desc: client SingleVerify ok test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentClientTest, AppDomainVerifyAgentClientTest002, TestSize.Level0)
{
    std::shared_ptr<AppDomainVerifyAgentRemoteStubMock> agentStubMock_ =
        std::make_shared<AppDomainVerifyAgentRemoteStubMock>();
    EXPECT_CALL(*agentStubMock_, SendRequest(_, _, _, _)).Times(1).WillOnce(::testing::Invoke(AgentInvokeOK));
    AppDomainVerifyAgentClient::agentServiceProxy_ = sptr<AppDomainVerifyAgentServiceProxy>::MakeSptr(
        agentStubMock_.get());
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfo.fingerprint = FINGERPRINT;
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    AppDomainVerifyAgentClient::GetInstance()->SingleVerify(appVerifyBaseInfo, skillUris);
    ASSERT_TRUE(g_enterAgentInvokeOK);
    AppDomainVerifyAgentClient::agentServiceProxy_.ForceSetRefPtr(nullptr);
}

/**
 * @tc.name: AppDomainVerifyAgentClientTest004
 * @tc.desc: SkillUri  test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentClientTest, AppDomainVerifyAgentClientTest003, TestSize.Level0)
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
    auto unmarshallingSkillUri = SkillUri::Unmarshalling(parcel);
    ASSERT_TRUE(unmarshallingSkillUri->host == "host");
}
/**
 * @tc.name: AppDomainVerifyAgentSaDeathRecipientTest001
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentClientTest, AppDomainVerifyAgentSaDeathRecipientTest001, TestSize.Level0)
{
    std::unique_ptr<AppDomainVerifyAgentRemoteStubMock> impl = std::make_unique<AppDomainVerifyAgentRemoteStubMock>();
    sptr<AppDomainVerifyAgentRemoteStubMock> mgrStubMock_(impl.get());
    EXPECT_CALL(*mgrStubMock_, AsObject()).Times(1).WillOnce(::testing::Return(mgrStubMock_));
    AppDomainVerifyAgentClient::agentServiceProxy_ = mgrStubMock_;
    AgentSaDeathRecipient AppDomainVerifyAgentSaDeathRecipient;
    AppDomainVerifyAgentSaDeathRecipient.OnRemoteDied(nullptr);
    ASSERT_TRUE(AppDomainVerifyAgentClient::agentServiceProxy_ == nullptr);
    mgrStubMock_.ForceSetRefPtr(nullptr);
    printf("reset mgrStubMock_ \n");
}

/**
 * @tc.name: AppDomainVerifyAgentSaDeathRecipientTest002
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentClientTest, AppDomainVerifyAgentSaDeathRecipientTest002, TestSize.Level0)
{
    std::unique_ptr<AppDomainVerifyAgentRemoteStubMock> impl = std::make_unique<AppDomainVerifyAgentRemoteStubMock>();
    sptr<AppDomainVerifyAgentRemoteStubMock> mgrStubMock_(impl.get());
    EXPECT_CALL(*mgrStubMock_, AsObject()).Times(1).WillOnce(::testing::Return(nullptr));
    AppDomainVerifyAgentClient::agentServiceProxy_ = mgrStubMock_;

    AgentSaDeathRecipient AppDomainVerifyAgentSaDeathRecipient;
    AppDomainVerifyAgentSaDeathRecipient.OnRemoteDied(nullptr);
    ASSERT_TRUE(AppDomainVerifyAgentClient::agentServiceProxy_ == nullptr);
    mgrStubMock_.ForceSetRefPtr(nullptr);
    printf("reset mgrStubMock_ \n");
}

/**
 * @tc.name: AppDomainVerifyAgentSaDeathRecipientTest003
 * @tc.desc: VerifyDomain test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentClientTest, AppDomainVerifyAgentSaDeathRecipientTest003, TestSize.Level0)
{
    AgentSaDeathRecipient AppDomainVerifyAgentSaDeathRecipient;
    AppDomainVerifyAgentSaDeathRecipient.OnRemoteDied(nullptr);
    ASSERT_TRUE(AppDomainVerifyAgentClient::agentServiceProxy_ == nullptr);
}
}
