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
#include "mock_http_client/http_client_error.h"
#include "mock_http_client/http_client_request.h"
#include "mock_http_client/http_client_response.h"
#include "mock_http_client/http_client_task.h"
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
    ffrt::this_task::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
}
/**
 * @tc.name: AppDomainVerifyTaskMgrTest001
 * @tc.desc: AddTask null test, return idle true
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
 * @tc.desc: add task with null http task
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest002, TestSize.Level0)
{
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    std::shared_ptr<MocVerifyHttpTask> task = std::make_shared<MocVerifyHttpTask>();
    EXPECT_CALL(*task, CreateHttpClientTask()).Times(1).WillOnce(Return(nullptr));
    EXPECT_CALL(*task, OnSuccess(_, _)).Times(0);
    EXPECT_CALL(*task, OnCancel(_, _)).Times(0);
    EXPECT_CALL(*task, OnFail(_, _, _)).Times(0);
    EXPECT_CALL(*task, OnDataReceive(_, _, _, _)).Times(0);
    appDomainVerifyTaskMgr->AddTask(task);
    while (!appDomainVerifyTaskMgr->IsIdle()) {
        Sleep();
    }
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest003
 * @tc.desc: test on success called
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest003, TestSize.Level0)
{
    HttpClientRequest request;
    HttpClientResponse response;
    response.result_ = "";
    std::shared_ptr<HttpClientTask> clientTask = std::make_shared<HttpClientTask>(request);
    clientTask->MockStatus(true, false, false);
    clientTask->SetResponse(response);
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    std::shared_ptr<MocVerifyHttpTask> task = std::make_shared<MocVerifyHttpTask>();
    EXPECT_CALL(*task, CreateHttpClientTask()).Times(1).WillOnce(Return(clientTask));
    EXPECT_CALL(*task, OnSuccess(_, _)).Times(1);
    EXPECT_CALL(*task, OnCancel(_, _)).Times(0);
    EXPECT_CALL(*task, OnFail(_, _, _)).Times(0);
    EXPECT_CALL(*task, OnDataReceive(_, _, _, _)).Times(0);
    appDomainVerifyTaskMgr->AddTask(task);
    while (!appDomainVerifyTaskMgr->IsIdle()) {
        Sleep();
    }
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest004
 * @tc.desc: test on fail called
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
    EXPECT_CALL(*task, OnSuccess(_, _)).Times(0);
    EXPECT_CALL(*task, OnCancel(_, _)).Times(0);
    EXPECT_CALL(*task, OnFail(_, _, _)).Times(1);
    EXPECT_CALL(*task, OnDataReceive(_, _, _, _)).Times(0);
    appDomainVerifyTaskMgr->AddTask(task);
    while (!appDomainVerifyTaskMgr->IsIdle()) {
        Sleep();
    }
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest005
 * @tc.desc: on date receive
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest005, TestSize.Level0)
{
    HttpClientRequest request;
    HttpClientResponse response;
    response.result_ = "";
    std::shared_ptr<HttpClientTask> clientTask = std::make_shared<HttpClientTask>(request);
    clientTask->MockStatus(true, false, true);
    clientTask->SetResponse(response);
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    std::shared_ptr<MocVerifyHttpTask> task = std::make_shared<MocVerifyHttpTask>();
    EXPECT_CALL(*task, CreateHttpClientTask()).Times(1).WillOnce(Return(clientTask));
    EXPECT_CALL(*task, OnSuccess(_, _)).Times(1);
    EXPECT_CALL(*task, OnCancel(_, _)).Times(0);
    EXPECT_CALL(*task, OnFail(_, _, _)).Times(0);
    EXPECT_CALL(*task, OnDataReceive(_, _, _, _)).Times(3);

    appDomainVerifyTaskMgr->AddTask(task);
    while (!appDomainVerifyTaskMgr->IsIdle()) {
        Sleep();
    }
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}
/**
 * @tc.name: AppDomainVerifyTaskMgrTest006
 * @tc.desc: on cancel
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
    EXPECT_CALL(*task, OnSuccess(_, _)).Times(0);
    EXPECT_CALL(*task, OnCancel(_, _)).Times(1);
    EXPECT_CALL(*task, OnFail(_, _, _)).Times(0);
    EXPECT_CALL(*task, OnDataReceive(_, _, _, _)).Times(0);
    appDomainVerifyTaskMgr->AddTask(task);
    while (!appDomainVerifyTaskMgr->IsIdle()) {
        Sleep();
    }
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}
class BaseHttpTask : public IHttpTask {
public:
    std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask> CreateHttpClientTask() override
    {
        return nullptr;
    }
    void OnSuccess(const HttpClientRequest& request, const HttpClientResponse& response) override
    {
    }
    void OnCancel(const HttpClientRequest& request, const HttpClientResponse& response) override
    {
    }
    void OnFail(
        const HttpClientRequest& request, const HttpClientResponse& response, const HttpClientError& error) override
    {
    }
    void OnDataReceive(std::shared_ptr<OHOS::NetStack::HttpClient::HttpClientTask> task,
        const HttpClientRequest& request, const uint8_t* data, size_t length) override
    {
    }
};
/**
 * @tc.name: AppDomainVerifyTaskMgrTest007
 * @tc.desc: test HttpTask
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest007, TestSize.Level0)
{
    std::shared_ptr<BaseHttpTask> baseHttpTask = std::make_shared<BaseHttpTask>();
    HttpClientRequest request;
    HttpClientResponse response;
    HttpClientError error;
    uint8_t* data;
    baseHttpTask->OnSuccess(request, response);
    baseHttpTask->OnFail(request, response, error);
    baseHttpTask->OnCancel(request, response);
    baseHttpTask->OnDataReceive(nullptr, request, data, 0);
    ASSERT_TRUE(baseHttpTask->CreateHttpClientTask() == nullptr);
}
/**
 * @tc.name: AppDomainVerifyTaskMgrTest008
 * @tc.desc: test GetInstance/DestroyInstance
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest008, TestSize.Level0)
{
    AppDomainVerifyTaskMgr::GetInstance();
    ASSERT_FALSE(AppDomainVerifyTaskMgr::GetInstance()->AddTask(nullptr));
    ASSERT_FALSE(AppDomainVerifyTaskMgr::GetInstance()->AddTask(nullptr));
    ASSERT_TRUE(AppDomainVerifyTaskMgr::GetInstance()->IsIdle());
    AppDomainVerifyTaskMgr::DestroyInstance();
}
/**
 * @tc.name: AppDomainVerifySafeMapTest001
 * @tc.desc: test GetInstance/DestroyInstance
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifySafeMapTest001, TestSize.Level0)
{
    SafeMap<int, std::string> safeMap;
    safeMap.EnsureInsert(1, "test");
    ASSERT_TRUE(safeMap.Size() == 1);
    ASSERT_FALSE(safeMap.IsEmpty());

    safeMap.Insert(2, "test");
    ASSERT_TRUE(safeMap.Size() == 2);

    std::string val;
    ASSERT_TRUE(safeMap.Find(1, val) == true);

    safeMap.Erase(2);
    ASSERT_TRUE(safeMap.Size() == 1);

    auto anotherSafeMap = safeMap;
    ASSERT_TRUE(anotherSafeMap.Size() == 1);

    SafeMap<int, std::string> map(safeMap);
    ASSERT_TRUE(map.Size() == 1);
    safeMap.Clear();
}
}