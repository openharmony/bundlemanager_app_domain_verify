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
#include "mock_constant.h"
#define private public
#define protected public
#include "verify_task.h"
#undef private
#undef protected
#include "moc_verify_task.h"
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class DomainVerifierTaskTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DomainVerifierTaskTest::SetUpTestCase(void)
{
}

void DomainVerifierTaskTest::TearDownTestCase(void)
{
}

void DomainVerifierTaskTest::SetUp(void)
{
}

void DomainVerifierTaskTest::TearDown(void)
{
}
/**
 * @tc.name: DomainVerifierTaskTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTaskTest, DomainVerifierTaskTest001, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = "";
    appVerifyBaseInfo.fingerprint = "";
    appVerifyBaseInfo.appIdentifier = "";
    SkillUri uri1;
    const std::vector<SkillUri> skillUris;
    VerifyResultInfo verifyResultInfo;
    VerifyTask task(TaskType::IMMEDIATE_TASK, appVerifyBaseInfo, skillUris);
    ASSERT_TRUE(task.GetType() == TaskType::IMMEDIATE_TASK);
    ASSERT_TRUE(task.GetTaskType() == TaskType::IMMEDIATE_TASK);
    ASSERT_TRUE(task.GetAppVerifyBaseInfo().bundleName == "");
    ASSERT_TRUE(task.GetUriVerifyMap().empty());
    ASSERT_TRUE(task.GetInnerUriVerifyMap().empty());
    ASSERT_FALSE(task.SaveDomainVerifyStatus("", verifyResultInfo));
}
/**
 * @tc.name: DomainVerifierTaskTest002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTaskTest, DomainVerifierTaskTest002, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = "";
    appVerifyBaseInfo.fingerprint = "";
    appVerifyBaseInfo.appIdentifier = "";
    std::vector<SkillUri> skillUris;
    SkillUri uri1;
    uri1.host = "test";
    uri1.scheme = "";
    skillUris.push_back(uri1);

    SkillUri uri2;
    uri2.host = "";
    uri2.scheme = "";
    skillUris.push_back(uri2);

    SkillUri uri3;
    uri3.host = "";
    uri3.scheme = "https";
    skillUris.push_back(uri3);

    SkillUri uri4;
    uri4.host = "test";
    uri4.scheme = "https";
    skillUris.push_back(uri4);

    SkillUri uri5;
    uri4.host = "test/a";
    uri4.scheme = "https";
    skillUris.push_back(uri5);

    SkillUri uri6;
    uri6.host = "";
    uri6.scheme = "https";
    skillUris.push_back(uri6);

    SkillUri uri7;
    uri7.host = "test";
    uri7.scheme = "https";
    skillUris.push_back(uri7);

    SkillUri uri8;
    uri8.host = "test/a";
    uri8.scheme = "https";
    skillUris.push_back(uri8);

    VerifyResultInfo verifyResultInfo;
    VerifyTask task(TaskType::IMMEDIATE_TASK, appVerifyBaseInfo, skillUris);
    ASSERT_TRUE(task.GetType() == TaskType::IMMEDIATE_TASK);
}
/**
 * @tc.name: DomainVerifierTaskTest003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTaskTest, DomainVerifierTaskTest003, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = "";
    appVerifyBaseInfo.fingerprint = "";
    appVerifyBaseInfo.appIdentifier = "";
    SkillUri uri1;
    uri1.scheme = "https";
    uri1.host = "e";
    const std::vector<SkillUri> skillUris;
    VerifyTask task(TaskType::IMMEDIATE_TASK, appVerifyBaseInfo, skillUris);
    OHOS::NetStack::HttpClient::HttpClientRequest request;
    std::string url = "";
    ASSERT_TRUE(task.OnPreRequest(request, url));
}
/**
 * @tc.name: DomainVerifierTaskTest004
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTaskTest, DomainVerifierTaskTest004, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = "";
    appVerifyBaseInfo.fingerprint = "";
    appVerifyBaseInfo.appIdentifier = "";
    SkillUri uri1;
    uri1.scheme = "https";
    uri1.host = "e";
    const std::vector<SkillUri> skillUris;
    VerifyTask task(TaskType::IMMEDIATE_TASK, appVerifyBaseInfo, skillUris);
    OHOS::NetStack::HttpClient::HttpClientResponse response;
    response.SetResponseCode(OHOS::NetStack::HttpClient::ResponseCode::OK);
    response.SetResult("OK");
    std::string url = "";
    task.OnPostVerify(url, response);
}
/**
 * @tc.name: DomainVerifierTaskSaveResultTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTaskTest, DomainVerifierTaskSaveResultTest001, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = "";
    appVerifyBaseInfo.fingerprint = "";
    appVerifyBaseInfo.appIdentifier = "";
    SkillUri uri1;
    uri1.scheme = "https";
    uri1.host = "e";
    const std::vector<SkillUri> skillUris;
    MocVerifyTask task(TaskType::IMMEDIATE_TASK, appVerifyBaseInfo, skillUris);
    OHOS::NetStack::HttpClient::HttpClientResponse response;
    response.SetResponseCode(OHOS::NetStack::HttpClient::ResponseCode::OK);
    response.SetResult("OK");
    std::string url = "";

    EXPECT_CALL(task, SaveDomainVerifyStatus(_, _)).Times(1).WillOnce(Return(true));

    task.OnSaveVerifyResult();
}
/**
 * @tc.name: DomainVerifierTaskSaveResultTest002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(DomainVerifierTaskTest, DomainVerifierTaskSaveResultTest002, TestSize.Level0)
{
    AppVerifyBaseInfo appVerifyBaseInfo;
    appVerifyBaseInfo.bundleName = "";
    appVerifyBaseInfo.fingerprint = "";
    appVerifyBaseInfo.appIdentifier = "";
    SkillUri uri1;
    uri1.scheme = "https";
    uri1.host = "e";
    const std::vector<SkillUri> skillUris;
    MocVerifyTask task(TaskType::IMMEDIATE_TASK, appVerifyBaseInfo, skillUris);
    OHOS::NetStack::HttpClient::HttpClientResponse response;
    response.SetResponseCode(OHOS::NetStack::HttpClient::ResponseCode::OK);
    response.SetResult("OK");
    std::string url = "";

    EXPECT_CALL(task, SaveDomainVerifyStatus(_, _)).Times(1).WillOnce(Return(false));

    task.OnSaveVerifyResult();
}
}