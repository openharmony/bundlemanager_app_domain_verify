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
#include <thread>
#include "../../mock/include/http_client/http_client_error.h"
#include "../../mock/include/http_client/http_client_request.h"
#include "../../mock/include/http_client/http_client_response.h"
#include "../../mock/include/http_client/http_client_task.h"
#define private public
#define protected public
#include "app_domain_verify_task_mgr.h"
#undef private
#undef protected
#include "mock_verify_http_task.h"

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
class AppDomainVerifyTaskMgrTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AppDomainVerifyTaskMgrTest::SetUpTestCase(void)
{
}

void AppDomainVerifyTaskMgrTest::TearDownTestCase(void)
{
}

void AppDomainVerifyTaskMgrTest::SetUp(void)
{
}

void AppDomainVerifyTaskMgrTest::TearDown(void)
{
}

#define SLEEP_TIME (10)
void Sleep()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
}
/**
 * @tc.name: AppDomainVerifyTaskMgrTest001
 * @tc.desc: AddTask test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest001, TestSize.Level0)
{
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    ASSERT_FALSE(appDomainVerifyTaskMgr->AddTask(nullptr));
    ASSERT_FALSE(appDomainVerifyTaskMgr->AddTask(nullptr));
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest002
 * @tc.desc: null http task
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest002, TestSize.Level0)
{
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    std::shared_ptr<MocVerifyHttpTask> task = std::make_shared<MocVerifyHttpTask>();
    EXPECT_CALL(*task, CreateHttpClientTask()).Times(1).WillOnce(Return(nullptr));
    appDomainVerifyTaskMgr->AddTask(task);
    Sleep();
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest003
 * @tc.desc: on success
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest003, TestSize.Level0)
{
    HttpClientRequest request;
    HttpClientResponse response;
    response.result_ = "";
    std::shared_ptr<HttpClientTask> clientTask = std::make_shared<HttpClientTask>(request);
    clientTask->MockStatus(false, true, false);
    clientTask->SetResponse(response);
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    std::shared_ptr<MocVerifyHttpTask> task = std::make_shared<MocVerifyHttpTask>();
    EXPECT_CALL(*task, CreateHttpClientTask()).Times(1).WillOnce(Return(clientTask));

    appDomainVerifyTaskMgr->AddTask(task);
    Sleep();
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest004
 * @tc.desc: on fail
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest004, TestSize.Level0)
{
    HttpClientRequest request;
    HttpClientResponse response;
    response.result_ = "";
    std::shared_ptr<HttpClientTask> clientTask = std::make_shared<HttpClientTask>(request);
    clientTask->MockStatus(false, true, false);
    clientTask->SetResponse(response);
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    std::shared_ptr<MocVerifyHttpTask> task = std::make_shared<MocVerifyHttpTask>();
    EXPECT_CALL(*task, CreateHttpClientTask()).Times(1).WillOnce(Return(clientTask));

    appDomainVerifyTaskMgr->AddTask(task);
    Sleep();
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}
/**
 * @tc.name: AppDomainVerifyTaskMgrTest005
 * @tc.desc: on date reciveve
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest005, TestSize.Level0)
{
    HttpClientRequest request;
    HttpClientResponse response;
    response.result_ = "";
    std::shared_ptr<HttpClientTask> clientTask = std::make_shared<HttpClientTask>(request);
    clientTask->MockStatus(false, true, true);
    clientTask->SetResponse(response);
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    std::shared_ptr<MocVerifyHttpTask> task = std::make_shared<MocVerifyHttpTask>();
    EXPECT_CALL(*task, CreateHttpClientTask()).Times(1).WillOnce(Return(clientTask));

    appDomainVerifyTaskMgr->AddTask(task);
    Sleep();
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}
/**
 * @tc.name: AppDomainVerifyTaskMgrTest006
 * @tc.desc: on date cancle
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest006, TestSize.Level0)
{
    HttpClientRequest request;
    HttpClientResponse response;
    response.result_ = "";
    std::shared_ptr<HttpClientTask> clientTask = std::make_shared<HttpClientTask>(request);
    clientTask->MockStatus(false, false, false, true);
    clientTask->SetResponse(response);
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    std::shared_ptr<MocVerifyHttpTask> task = std::make_shared<MocVerifyHttpTask>();
    EXPECT_CALL(*task, CreateHttpClientTask()).Times(1).WillOnce(Return(clientTask));

    appDomainVerifyTaskMgr->AddTask(task);
    Sleep();
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}
}