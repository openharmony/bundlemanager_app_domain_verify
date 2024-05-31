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
#include "../../mock/include/http_client/http_client_error.h"
#include "../../mock/include/http_client/http_client_request.h"
#include "../../mock/include/http_client/http_client_response.h"
#include "../../mock/include/http_client/http_client_task.h"
#include "app_domain_verify_mgr_client.h"
#include "mock_constant.h"
#include "mock_verify_agent.h"
#include "agent_interface_code.h"
#define private public
#define protected public
#include "app_domain_verify_agent_client.h"
#include "app_domain_verify_agent_service.h"
#include "app_domain_verify_mgr_service.h"
#undef private
#undef protected

namespace OHOS::AppDomainVerify {
using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using namespace testing;
using namespace testing::ext;
class AppDomainVerifyAgentModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AppDomainVerifyAgentModuleTest::SetUpTestCase(void)
{
    printf("SetUpTestCase \n");
}

void AppDomainVerifyAgentModuleTest::TearDownTestCase(void)
{
    printf("TearDownTestCase \n");
}

void AppDomainVerifyAgentModuleTest::SetUp(void)
{
    printf("SetUp \n");
    AppDomainVerifyMgrClient::GetInstance()->ClearDomainVerifyStatus(APP_IDENTIFIER, BUNDLE_NAME);
}

void AppDomainVerifyAgentModuleTest::TearDown(void)
{
    printf("TearDown \n");
}

static void Sleep(int milliseconds = 10)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

std::string jsonString =
    R"({"applinking":{"apps":[{"appIdentifier":"appIdentifier","bundleName":"com.openHarmony.test","fingerprint":"fingerprint"}]}})";

/**
 * @tc.name: AppDomainVerifyAgentModuleTest001
 * @tc.desc: SingleVerify bundleName fingerprint ok test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentModuleTest, AppDomainVerifyAgentModuleTest001, TestSize.Level0)
{
    std::shared_ptr<AppDomainVerifyAgentService> appDomainVerifyAgentService =
        std::make_shared<AppDomainVerifyAgentService>();
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfo.fingerprint = FINGERPRINT;
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    appDomainVerifyAgentService->SingleVerify(appVerifyBaseInfo, skillUris);
    Sleep();
    DomainVerifyStatus domainVerificationState;
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryDomainVerifyStatus(
        BUNDLE_NAME, domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}

/**
 * @tc.name: AppDomainVerifyAgentModuleTest004
 * @tc.desc: SingleVerify bundleName fingerprint fail test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentModuleTest, AppDomainVerifyAgentModuleTest004, TestSize.Level0)
{
    HttpClientTask::MockStatus();
    HttpClientResponse response;
    response.responseCode_ = 200;
    response.result_ = jsonString;
    std::shared_ptr<AppDomainVerifyAgentService> appDomainVerifyAgentService =
        std::make_shared<AppDomainVerifyAgentService>();
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfo.fingerprint = "wrong fingerprint";
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    appDomainVerifyAgentService->SingleVerify(appVerifyBaseInfo, skillUris);
    Sleep();
    DomainVerifyStatus domainVerificationState;
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryDomainVerifyStatus(
        BUNDLE_NAME, domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}

/**
 * @tc.name: AppDomainVerifyAgentModuleTest005
 * @tc.desc: SingleVerify appIdentifier fail test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentModuleTest, AppDomainVerifyAgentModuleTest005, TestSize.Level0)
{
    HttpClientTask::MockStatus();
    HttpClientResponse response;
    response.responseCode_ = 200;
    response.result_ = jsonString;
    std::shared_ptr<AppDomainVerifyAgentService> appDomainVerifyAgentService =
        std::make_shared<AppDomainVerifyAgentService>();
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfo.fingerprint = "wrong fingerprint";
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    appDomainVerifyAgentService->SingleVerify(appVerifyBaseInfo, skillUris);
    Sleep();
    DomainVerifyStatus domainVerificationState;
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryDomainVerifyStatus(
        BUNDLE_NAME, domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}

/**
 * @tc.name: AppDomainVerifyAgentModuleTest006
 * @tc.desc: SingleVerify appIdentifier fail test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentModuleTest, AppDomainVerifyAgentModuleTest006, TestSize.Level0)
{
    HttpClientTask::MockStatus();
    HttpClientResponse response;
    response.responseCode_ = 200;
    response.result_ = jsonString;
    std::shared_ptr<AppDomainVerifyAgentService> appDomainVerifyAgentService =
        std::make_shared<AppDomainVerifyAgentService>();
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfo.fingerprint = "wrong fingerprint";
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    appDomainVerifyAgentService->SingleVerify(appVerifyBaseInfo, skillUris);
    Sleep();
    DomainVerifyStatus domainVerificationState;
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryDomainVerifyStatus(
        BUNDLE_NAME, domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}

/**
 * @tc.name: AppDomainVerifyAgentModuleTest007
 * @tc.desc: SingleVerify network error test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentModuleTest, AppDomainVerifyAgentModuleTest007, TestSize.Level0)
{
    HttpClientTask::MockStatus(false, true);
    HttpClientResponse response;
    response.responseCode_ = 200;
    response.result_ = jsonString;
    std::shared_ptr<AppDomainVerifyAgentService> appDomainVerifyAgentService =
        std::make_shared<AppDomainVerifyAgentService>();
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfo.fingerprint = FINGERPRINT;
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    appDomainVerifyAgentService->SingleVerify(appVerifyBaseInfo, skillUris);
    Sleep();
    DomainVerifyStatus domainVerificationState;
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryDomainVerifyStatus(
        BUNDLE_NAME, domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}

/**
 * @tc.name: AppDomainVerifyAgentModuleTest008
 * @tc.desc: SingleVerify wrong fingerprint test.
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyAgentModuleTest, AppDomainVerifyAgentModuleTest008, TestSize.Level0)
{
    HttpClientTask::MockStatus();
    HttpClientResponse response;
    response.responseCode_ = 200;
    response.result_ = jsonString;
    std::shared_ptr<AppDomainVerifyAgentService> appDomainVerifyAgentService =
        std::make_shared<AppDomainVerifyAgentService>();
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.appIdentifier = APP_IDENTIFIER;
    appVerifyBaseInfo.bundleName = BUNDLE_NAME;
    appVerifyBaseInfo.fingerprint = "wrong fingerprint";
    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUri.scheme = "https";
    skillUri.host = HOST;
    skillUris.emplace_back(skillUri);
    appDomainVerifyAgentService->SingleVerify(appVerifyBaseInfo, skillUris);
    Sleep();

    DomainVerifyStatus domainVerificationState;
    auto queryRes = AppDomainVerifyMgrClient::GetInstance()->QueryDomainVerifyStatus(
        BUNDLE_NAME, domainVerificationState);
    ASSERT_TRUE(queryRes);
    ASSERT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}
}
