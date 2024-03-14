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
#define private public
#define protected public
#include "app_domain_verify_task_mgr.h"
#undef private
#undef protected
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
    std::unordered_map<std::string, InnerVerifyStatus> uriVerifyMap;
    auto task = std::make_shared<Task>(TaskType::UNKNOWN_TASK, appVerifyBaseInfo, uriVerifyMap);
    ASSERT_FALSE(appDomainVerifyTaskMgr->AddTask(task));
} 

/**
 * @tc.name: AppDomainVerifyTaskMgrTest002
 * @tc.desc: IsIdle test
 * @tc.type: FUNC
*/
HWTEST_F(AppDomainVerifyTaskMgrTest, AppDomainVerifyTaskMgrTest002, TestSize.Level0)
{
    auto appDomainVerifyTaskMgr = std::make_shared<AppDomainVerifyTaskMgr>();
    appDomainVerifyTaskMgr->taskRunning = true;
    appDomainVerifyTaskMgr->Run();
    AppVerifyBaseInfo appVerifyBaseInfo;
    std::unordered_map<std::string, InnerVerifyStatus> uriVerifyMap;
    auto task = std::make_shared<Task>(TaskType::IMMEDIATE_TASK, appVerifyBaseInfo, uriVerifyMap);
    appDomainVerifyTaskMgr->AddTask(task);
    ASSERT_FALSE(appDomainVerifyTaskMgr->IsIdle());
} 
}