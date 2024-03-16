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
#include "mock_constant.h"
#include "rdb_helper.h"
#include "mock_verify_mgr.h"
#define private public
#define protected public
#include "app_domain_verify_agent_service.h"
#undef private
#undef protected
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class AgentServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AgentServiceTest::SetUpTestCase(void)
{
}

void AgentServiceTest::TearDownTestCase(void)
{
}

void AgentServiceTest::SetUp(void)
{
}

void AgentServiceTest::TearDown(void)
{
}

/**
 * @tc.name: AgentServiceTest001
 * @tc.desc: CompleteVerifyRefresh test.
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest001, TestSize.Level0)
{
    auto appDomainVerifyAgentService = std::make_shared<AppDomainVerifyAgentService>();
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_FAIL);
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.insert_or_assign(BUNDLE_NAME, verifyResultInfo);
    std::vector<InnerVerifyStatus> statuses = std::vector<InnerVerifyStatus>{ InnerVerifyStatus::UNKNOWN,
        InnerVerifyStatus::STATE_FAIL, InnerVerifyStatus::FAILURE_REDIRECT, InnerVerifyStatus::FAILURE_CLIENT_ERROR,
        InnerVerifyStatus::FAILURE_REJECTED_BY_SERVER, InnerVerifyStatus::FAILURE_HTTP_UNKNOWN,
        InnerVerifyStatus::FAILURE_TIMEOUT };
    appDomainVerifyAgentService->CompleteVerifyRefresh(bundleVerifyStatusInfo, statuses, 1, TaskType::IMMEDIATE_TASK);
}

/**
 * @tc.name: AgentServiceTest002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest002, TestSize.Level0)
{
    auto appDomainVerifyAgentService = std::make_shared<AppDomainVerifyAgentService>();
    SystemAbilityOnDemandReason startReason;
    appDomainVerifyAgentService->OnStart(startReason);
    SystemAbilityOnDemandReason idleReason;
    appDomainVerifyAgentService->OnIdle(idleReason);
    appDomainVerifyAgentService->ExitIdleState();
    appDomainVerifyAgentService->OnStop();
}
}
