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
#include <memory>
#include "mock_constant.h"
#include "rdb_helper.h"
#include "mock_verify_mgr.h"
#include "parcel_util.h"
#include "app_domain_verify_agent_service_proxy.h"
#define private public
#define protected public
#include "app_domain_verify_agent_service.h"
#include "app_domain_verify_agent_service_proxy.h"
#undef private
#undef protected
#include "mock_http_client_task.h"
#include "mock_http_client_task_factory.h"
#include "mock_constant.h"
#include "mock_verify_agent.h"
#include "agent_interface_code.h"
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
bool MocHttpClientTask::sTaskRunOk;
bool MocHttpClientTask::sHttpOk;
bool MocHttpClientTask::sIsDataRecv = false;
bool MocHttpClientTask::sIsCancel = false;
const std::string BOOT_COMPLETED_EVENT = "usual.event.BOOT_COMPLETED";
const std::string LOOP_EVENT = "loopevent";
std::shared_ptr<AppDomainVerifyAgentService> appDomainVerifyAgentService =
    std::make_shared<AppDomainVerifyAgentService>();
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
std::shared_ptr<VerifyHttpTask> InvokeGetTask(const OHOS::NetStack::HttpClient::HttpClientRequest& httpClientRequest)
{
    return std::make_shared<MocHttpClientTask>(httpClientRequest);
}
/**
 * @tc.name: AgentServiceTest001
 * @tc.desc: CompleteVerifyRefresh test.
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest001, TestSize.Level0)
{
    BundleVerifyStatusInfo bundleVerifyStatusInfo;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_FAIL);
    bundleVerifyStatusInfo.bundleVerifyStatusInfoMap_.insert_or_assign(BUNDLE_NAME, verifyResultInfo);
    std::vector<InnerVerifyStatus> statuses = std::vector<InnerVerifyStatus>{ InnerVerifyStatus::UNKNOWN,
        InnerVerifyStatus::STATE_FAIL, InnerVerifyStatus::FAILURE_REDIRECT, InnerVerifyStatus::FAILURE_CLIENT_ERROR,
        InnerVerifyStatus::FAILURE_REJECTED_BY_SERVER, InnerVerifyStatus::FAILURE_HTTP_UNKNOWN,
        InnerVerifyStatus::FAILURE_TIMEOUT };
    appDomainVerifyAgentService->CompleteVerifyRefresh(bundleVerifyStatusInfo, statuses, 1, TaskType::IMMEDIATE_TASK);
    appDomainVerifyAgentService->CompleteVerifyRefresh(bundleVerifyStatusInfo, statuses, 0, TaskType::IMMEDIATE_TASK);
}

/**
 * @tc.name: AgentServiceTest003
 * @tc.desc: CompleteVerifyRefresh test.
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest003, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    std::vector<SkillUri> skillUris;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, std::u16string(u"abc"));
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &appVerifyBaseInfo);
    uint32_t size = static_cast<uint32_t>(skillUris.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, size);
    for (uint32_t i = 0; i < skillUris.size(); ++i) {
        WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &skillUris[i]);
    }
    int32_t error = appDomainVerifyAgentService->OnRemoteRequest(
        AgentInterfaceCode::SINGLE_VERIFY, data, reply, option);
    ASSERT_TRUE(error != 0);
}
/**
 * @tc.name: AgentServiceTest004
 * @tc.desc: CompleteVerifyRefresh test.
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest004, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    std::vector<SkillUri> skillUris;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyAgentServiceStub::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &appVerifyBaseInfo);
    uint32_t size = static_cast<uint32_t>(skillUris.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, size);
    for (uint32_t i = 0; i < skillUris.size(); ++i) {
        WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &skillUris[i]);
    }
    int32_t error = appDomainVerifyAgentService->OnRemoteRequest(5, data, reply, option);
    ASSERT_TRUE(error != 0);
}
/**
 * @tc.name: AgentServiceTest005
 * @tc.desc: CompleteVerifyRefresh test.
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest005, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    std::vector<SkillUri> skillUris;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyAgentServiceStub::GetDescriptor());
    uint32_t size = static_cast<uint32_t>(skillUris.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, size);
    for (uint32_t i = 0; i < skillUris.size(); ++i) {
        WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &skillUris[i]);
    }
    int32_t error = appDomainVerifyAgentService->OnRemoteRequest(
        AgentInterfaceCode::SINGLE_VERIFY, data, reply, option);
    ASSERT_TRUE(error != 0);
}
/**
 * @tc.name: AgentServiceTest006
 * @tc.desc: ok test.
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest006, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    std::vector<SkillUri> skillUris;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyAgentServiceStub::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &appVerifyBaseInfo);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, 2);
    int32_t error = appDomainVerifyAgentService->OnRemoteRequest(
        AgentInterfaceCode::SINGLE_VERIFY, data, reply, option);
    ASSERT_TRUE(error != 0);
}

/**
 * @tc.name: AgentServiceTest020
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest020, TestSize.Level0)
{
    SystemAbilityOnDemandReason startReason;
    appDomainVerifyAgentService->OnStart(startReason);

    startReason.SetName(BOOT_COMPLETED_EVENT);
    appDomainVerifyAgentService->OnStart(startReason);

    startReason.SetName(LOOP_EVENT);
    appDomainVerifyAgentService->OnStart(startReason);

    startReason.SetName("other");
    appDomainVerifyAgentService->OnStart(startReason);

    SystemAbilityOnDemandReason idleReason;
    appDomainVerifyAgentService->OnIdle(idleReason);
    idleReason.SetName(BOOT_COMPLETED_EVENT);
    appDomainVerifyAgentService->OnIdle(idleReason);
    appDomainVerifyAgentService->ExitIdleState();
    appDomainVerifyAgentService->OnStop();
}
/**
 * @tc.name: AgentServiceTest021
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest021, TestSize.Level0)
{
    SystemAbilityOnDemandReason startReason;
    startReason.SetName(BOOT_COMPLETED_EVENT);
    appDomainVerifyAgentService->OnStart(startReason);
    SystemAbilityOnDemandReason idleReason;
    appDomainVerifyAgentService->OnIdle(idleReason);
    idleReason.SetName(BOOT_COMPLETED_EVENT);
    appDomainVerifyAgentService->OnIdle(idleReason);
    appDomainVerifyAgentService->ExitIdleState();
    appDomainVerifyAgentService->OnStop();
}
/**
 * @tc.name: AgentServiceTest022
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest022, TestSize.Level0)
{
    SystemAbilityOnDemandReason startReason;
    startReason.SetName(LOOP_EVENT);
    appDomainVerifyAgentService->OnStart(startReason);
    SystemAbilityOnDemandReason idleReason;
    appDomainVerifyAgentService->OnIdle(idleReason);
    idleReason.SetName(BOOT_COMPLETED_EVENT);
    appDomainVerifyAgentService->OnIdle(idleReason);
    appDomainVerifyAgentService->ExitIdleState();
    appDomainVerifyAgentService->OnStop();
}
/**
 * @tc.name: AgentServiceTest023
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest023, TestSize.Level0)
{
    SystemAbilityOnDemandReason startReason;
    startReason.SetName(LOOP_EVENT);
    appDomainVerifyAgentService->OnStart(startReason);
    SystemAbilityOnDemandReason idleReason;
    appDomainVerifyAgentService->appDomainVerifyTaskMgr_ = nullptr;
    appDomainVerifyAgentService->OnIdle(idleReason);
    idleReason.SetName(BOOT_COMPLETED_EVENT);
    appDomainVerifyAgentService->OnIdle(idleReason);
    appDomainVerifyAgentService->ExitIdleState();
    appDomainVerifyAgentService->OnStop();
}
/**
 * @tc.name: AgentServiceTest024
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(AgentServiceTest, AgentServiceTest024, TestSize.Level0)
{
    SystemAbilityOnDemandReason startReason;
    startReason.SetName("other");
    appDomainVerifyAgentService->OnStart(startReason);
    SystemAbilityOnDemandReason idleReason;
    appDomainVerifyAgentService->appDomainVerifyTaskMgr_ = nullptr;
    appDomainVerifyAgentService->OnIdle(idleReason);
    idleReason.SetName(BOOT_COMPLETED_EVENT);
    appDomainVerifyAgentService->OnIdle(idleReason);
    appDomainVerifyAgentService->ExitIdleState();
    appDomainVerifyAgentService->OnStop();
}
}
