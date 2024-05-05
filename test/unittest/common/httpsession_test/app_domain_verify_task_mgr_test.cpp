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
#include "app_domain_verify_hisysevent.h"
#define private public
#define protected public
#include "app_domain_verify_task_mgr.h"
#include "moc_verify_task.h"
#include "skill_uri.h"
#undef private
#undef protected
#include "mock_http_client_task_factory.h"
#include "mock_http_client_task.h"
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
bool MocHttpClientTask::sTaskRunOk = false;
bool MocHttpClientTask::sHttpOk = false;
bool MocHttpClientTask::sIsDataRecv = false;
bool MocHttpClientTask::sIsCancel = false;
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
/**
 * @tc.name: AppDomainVerifyTaskMgrTest001
 * @tc.desc: AddTask test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest001, TestSize.Level0)
{
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    ASSERT_FALSE(appDomainVerifyTaskMgr->AddTask(nullptr));
    AppVerifyBaseInfo appVerifyBaseInfo;
    std::vector<SkillUri> skillUris;
    ASSERT_FALSE(appDomainVerifyTaskMgr->AddTask(nullptr));
    ASSERT_TRUE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest002
 * @tc.desc: IsIdle test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest002, TestSize.Level0)
{
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    appDomainVerifyTaskMgr->taskRunning_ = true;
    appDomainVerifyTaskMgr->Run();
    AppVerifyBaseInfo appVerifyBaseInfo;
    std::vector<SkillUri> skillUris;
    auto task = std::make_shared<MocVerifyTask>();
    EXPECT_CALL(*task, GetType()).WillOnce(Return(SCHEDULE_REFRESH_TASK));
    appDomainVerifyTaskMgr->AddTask(task);
    ASSERT_FALSE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest003
 * @tc.desc: IsIdle test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest003, TestSize.Level0)
{
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    appDomainVerifyTaskMgr->taskRunning_ = true;
    appDomainVerifyTaskMgr->Run();
    AppVerifyBaseInfo appVerifyBaseInfo;
    std::vector<SkillUri> skillUris;
    auto task = std::make_shared<MocVerifyTask>();
    EXPECT_CALL(*task, GetType()).Times(2).WillOnce(Return(UNKNOWN_TASK));
    appDomainVerifyTaskMgr->AddTask(task);
    ASSERT_FALSE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest004
 * @tc.desc: IsIdle test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest004, TestSize.Level0)
{
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    appDomainVerifyTaskMgr->taskRunning_ = true;
    appDomainVerifyTaskMgr->Run();
    AppVerifyBaseInfo appVerifyBaseInfo;
    std::vector<SkillUri> skillUris;
    auto task = std::make_shared<MocVerifyTask>();
    EXPECT_CALL(*task, GetType()).Times(2).WillOnce(Return(UNKNOWN_TASK));
    appDomainVerifyTaskMgr->AddTask(task);
    ASSERT_FALSE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest005
 * @tc.desc: IsIdle test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest005, TestSize.Level0)
{
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    appDomainVerifyTaskMgr->taskRunning_ = true;
    appDomainVerifyTaskMgr->taskQueue_.push_back(nullptr);
    appDomainVerifyTaskMgr->Run();
    ASSERT_FALSE(appDomainVerifyTaskMgr->IsIdle());
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest006
 * @tc.desc: IsIdle test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest006, TestSize.Level0)
{
    MocHttpClientTask::sTaskRunOk = true;
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    OHOS::NetStack::HttpClient::HttpClientRequest request;
    auto mockTask = std::make_shared<MocHttpClientTask>(request);
    auto httpClientTaskFactory_ = std::make_unique<MocHttpClientTaskFactory>();
    EXPECT_CALL(*httpClientTaskFactory_, CreateTask(_)).Times(1).WillOnce(Return(mockTask));
    appDomainVerifyTaskMgr->httpClientTaskFactory_ = std::move(httpClientTaskFactory_);

    auto task = std::make_shared<MocVerifyTask>();
    task->map_.emplace("aaa", InnerVerifyStatus::UNKNOWN);
    EXPECT_CALL(*task, GetType()).Times(1).WillOnce(Return(SCHEDULE_REFRESH_TASK));
    EXPECT_CALL(*task, OnPostVerify(_, _)).Times(1);
    EXPECT_CALL(*task, OnSaveVerifyResult()).Times(1);
    EXPECT_CALL(*task, OnPreRequest(_, _)).Times(1).WillOnce(Return(true));

    appDomainVerifyTaskMgr->AddTask(task);
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest007
 * @tc.desc: IsIdle test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest007, TestSize.Level0)
{
    auto task = std::make_shared<MocVerifyTask>();
    task->map_.emplace("aaa", InnerVerifyStatus::UNKNOWN);
    printf("moc address:%p\n", task.get());
    EXPECT_CALL(*task, GetType()).Times(1).WillOnce(Return(SCHEDULE_REFRESH_TASK));
    EXPECT_CALL(*task, OnPostVerify(_, _)).Times(1);
    EXPECT_CALL(*task, OnSaveVerifyResult()).Times(1);
    EXPECT_CALL(*task, OnPreRequest(_, _)).Times(1).WillOnce(Return(true));
    MocHttpClientTask::sTaskRunOk = false;
    MocHttpClientTask::sHttpOk = true;

    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    OHOS::NetStack::HttpClient::HttpClientRequest request;
    auto mockTask = std::make_shared<MocHttpClientTask>(request);
    auto httpClientTaskFactory_ = std::make_unique<MocHttpClientTaskFactory>();
    EXPECT_CALL(*httpClientTaskFactory_, CreateTask(_)).Times(1).WillOnce(Return(mockTask));
    appDomainVerifyTaskMgr->httpClientTaskFactory_ = std::move(httpClientTaskFactory_);
    appDomainVerifyTaskMgr->AddTask(task);
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest008
 * @tc.desc: IsIdle test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest008, TestSize.Level0)
{
    MocHttpClientTask::sTaskRunOk = false;
    MocHttpClientTask::sHttpOk = false;
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    OHOS::NetStack::HttpClient::HttpClientRequest request;
    auto mockTask = std::make_shared<MocHttpClientTask>(request);
    auto httpClientTaskFactory_ = std::make_unique<MocHttpClientTaskFactory>();
    EXPECT_CALL(*httpClientTaskFactory_, CreateTask(_)).Times(1).WillOnce(Return(mockTask));
    appDomainVerifyTaskMgr->httpClientTaskFactory_ = std::move(httpClientTaskFactory_);

    auto task = std::make_shared<MocVerifyTask>();
    task->map_.emplace("aaa", InnerVerifyStatus::UNKNOWN);
    EXPECT_CALL(*task, GetType()).Times(1).WillOnce(Return(SCHEDULE_REFRESH_TASK));
    EXPECT_CALL(*task, OnPostVerify(_, _)).Times(1);
    EXPECT_CALL(*task, OnSaveVerifyResult()).Times(1);
    EXPECT_CALL(*task, OnPreRequest(_, _)).Times(1).WillOnce(Return(true));

    appDomainVerifyTaskMgr->AddTask(task);
}

/**
 * @tc.name: AppDomainVerifyTaskMgrTest009
 * @tc.desc: IsIdle test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest009, TestSize.Level0)
{
    MocHttpClientTask::sTaskRunOk = false;
    MocHttpClientTask::sHttpOk = false;
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    auto task = std::make_shared<MocVerifyTask>();
    task->map_.emplace("aaa", InnerVerifyStatus::UNKNOWN);
    EXPECT_CALL(*task, GetType()).Times(1).WillOnce(Return(SCHEDULE_REFRESH_TASK));
    EXPECT_CALL(*task, OnPreRequest(_, _)).Times(1).WillOnce(Return(false));

    appDomainVerifyTaskMgr->AddTask(task);
}
/**
 * @tc.name: AppDomainVerifyTaskMgrTest010
 * @tc.desc: IsIdle test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest010, TestSize.Level0)
{
    MocHttpClientTask::sIsCancel = true;
    MocHttpClientTask::sIsDataRecv = true;

    MocHttpClientTask::sTaskRunOk = true;
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    OHOS::NetStack::HttpClient::HttpClientRequest request;
    auto mockTask = std::make_shared<MocHttpClientTask>(request);
    auto httpClientTaskFactory_ = std::make_unique<MocHttpClientTaskFactory>();
    EXPECT_CALL(*httpClientTaskFactory_, CreateTask(_)).Times(1).WillOnce(Return(mockTask));
    appDomainVerifyTaskMgr->httpClientTaskFactory_ = std::move(httpClientTaskFactory_);

    auto task = std::make_shared<MocVerifyTask>();
    task->map_.emplace("aaa", InnerVerifyStatus::UNKNOWN);
    EXPECT_CALL(*task, GetType()).Times(1).WillOnce(Return(SCHEDULE_REFRESH_TASK));
    EXPECT_CALL(*task, OnPostVerify(_, _)).Times(1);
    EXPECT_CALL(*task, OnSaveVerifyResult()).Times(1);
    EXPECT_CALL(*task, OnPreRequest(_, _)).Times(1).WillOnce(Return(true));

    appDomainVerifyTaskMgr->AddTask(task);
}
}