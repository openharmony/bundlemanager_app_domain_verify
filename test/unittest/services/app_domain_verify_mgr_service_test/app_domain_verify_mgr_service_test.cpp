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
#include <string>
#define private public
#define protected public
#include "app_domain_verify_data_mgr.h"
#include "app_domain_verify_mgr_service.h"
#include "app_domain_verify_agent_client.h"
#undef private
#undef protected
#include "mock_verify_agent.h"
#include "mock_constant.h"
#include "mock_verify_mgr.h"
#include "app_domain_verify_mgr_interface_code.h"
#include "rdb_helper.h"
#include "app_domain_verify_mgr_service_proxy.h"
#include "app_domain_verify_mgr_interface_code.h"
#include "system_ability_definition.h"
#include "parcel_util.h"

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
std::shared_ptr<AppDomainVerifyMgrService> appDomainVerifyMgrService = std::make_shared<AppDomainVerifyMgrService>();
auto appDomainVerifyAgentStubMock_ = std::make_shared<AppDomainVerifyAgentRemoteStubMock>();
class MgrServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

int InvokeSingleVerifyOK(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "%s call end", __func__);
    std::string bundleName = BUNDLE_NAME;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);
    appDomainVerifyMgrService->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
    return 0;
}

int InvokeSingleVerifyFail(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "%s call end", __func__);
    std::string bundleName = BUNDLE_NAME;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_FAIL);
    appDomainVerifyMgrService->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
    return UNKNOWN_ERROR;
}
void MgrServiceTest::SetUpTestCase(void)
{
    printf("SetUpTestCase \n");
}

void MgrServiceTest::TearDownTestCase(void)
{
    printf("TearDownTestCase  1\n");
    AppDomainVerifyAgentClient::agentServiceProxy_.ForceSetRefPtr(nullptr);
    printf("TearDownTestCase 2\n");
    AppDomainVerifyAgentClient::DestroyInstance();
    printf("TearDownTestCase 3\n");
    appDomainVerifyMgrService->Stop();
    appDomainVerifyAgentStubMock_.reset();
    printf("TearDownTestCase \n");
}

void MgrServiceTest::SetUp(void)
{
    printf("SetUp \n");
}

void MgrServiceTest::TearDown(void)
{
    printf("TearDown \n");
}

/**
 * @tc.name: MgrServiceVerifyDomainTest001
 * @tc.desc: verify domain ok
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceVerifyDomainTest001, TestSize.Level0)
{
    EXPECT_CALL(*appDomainVerifyAgentStubMock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(::testing::Invoke(InvokeSingleVerifyOK));
    AppDomainVerifyAgentClient::agentServiceProxy_ = sptr<AppDomainVerifyAgentServiceProxy>::MakeSptr(
            appDomainVerifyAgentStubMock_.get());

    std::string appIdentifier = "appIdentifier";
    std::string bundleName = "bundleName";
    std::string fingerprint = "fingerprint";

    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUris.push_back(skillUri);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, appIdentifier);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, fingerprint);

    uint32_t size = static_cast<uint32_t>(skillUris.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, size);

    for (uint32_t i = 0; i < skillUris.size(); ++i) {
        WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &skillUris[i]);
    }
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}

/**
 * @tc.name: MgrServiceVerifyDomainTest002
 * @tc.desc: verify domain wrong with interface token
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceVerifyDomainTest002, TestSize.Level0)
{
    std::string appIdentifier = "appIdentifier";
    std::string bundleName = "bundleName";
    std::string fingerprint = "fingerprint";

    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUris.push_back(skillUri);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, std::u16string(u"abc"));
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, appIdentifier);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, fingerprint);

    uint32_t size = static_cast<uint32_t>(skillUris.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, size);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN, data, reply, option);
    ASSERT_TRUE(error != ERR_OK);
}

/**
 * @tc.name: MgrServiceVerifyDomainTest003
 * @tc.desc: OnRemoteRequest with wrong opconde
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceVerifyDomainTest003, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    std::string appIdentifier = "appIdentifier";
    std::string bundleName = "bundleName";
    std::string fingerprint = "fingerprint";

    std::vector<SkillUri> skillUris;
    SkillUri skillUri;
    skillUris.push_back(skillUri);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, appIdentifier);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, fingerprint);

    uint32_t size = static_cast<uint32_t>(skillUris.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, size);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(100, data, reply, option);
    ASSERT_TRUE(error != ERR_OK);
}
/**
 * @tc.name: MgrServiceClearDomainTest001
 * @tc.desc: clear domain verify result ok
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceClearDomainTest001, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    std::string appIdentifier = "appIdentifier";
    std::string bundleName = "bundleName";
    std::string fingerprint = "fingerprint";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, appIdentifier);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::CLEAR_DOMAIN_VERIFY_RESULT, data, reply,
        option);
    ASSERT_TRUE(error == ERR_OK);
}

/**
 * @tc.name: MgrServiceFilterDomainTest001
 * @tc.desc: filter abilities ok
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceFilterDomainTest001, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    OHOS::AAFwk::Want want;
    std::vector<OHOS::AppExecFwk::AbilityInfo> originAbilityInfos;
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    originAbilityInfos.push_back(abilityInfo);
    std::vector<OHOS::AppExecFwk::AbilityInfo> filtedAbilityInfos;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &want);
    uint32_t originAbilityInfoSize = static_cast<uint32_t>(originAbilityInfos.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, originAbilityInfoSize);

    for (uint32_t i = 0; i < originAbilityInfos.size(); ++i) {
        WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &originAbilityInfos[i]);
    }
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply,
        option);
    ASSERT_TRUE(error == ERR_OK);
}

/**
 * @tc.name: MgrServiceFilterDomainTest002
 * @tc.desc: filter abilities with wrong originAbility array
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceFilterDomainTest002, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    OHOS::AAFwk::Want want;
    OHOS::AppExecFwk::ElementName element;
    element.SetBundleName(BUNDLE_NAME);
    element.SetAbilityName("a");
    want.SetElement(element);
    std::vector<OHOS::AppExecFwk::AbilityInfo> originAbilityInfos;
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    originAbilityInfos.push_back(abilityInfo);
    std::vector<OHOS::AppExecFwk::AbilityInfo> filtedAbilityInfos;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &want);
    uint32_t originAbilityInfoSize = static_cast<uint32_t>(originAbilityInfos.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, originAbilityInfoSize);
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply,
        option);
    ASSERT_TRUE(error != ERR_OK);
}

/**
 * @tc.name: MgrServiceFilterDomainTest003
 * @tc.desc: filter abilities with wrong originAbility size
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceFilterDomainTest003, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    OHOS::AAFwk::Want want;
    want.SetUri("https://www.baidu.com");

    std::vector<OHOS::AppExecFwk::AbilityInfo> originAbilityInfos;
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    originAbilityInfos.push_back(abilityInfo);
    std::vector<OHOS::AppExecFwk::AbilityInfo> filtedAbilityInfos;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &want);
    uint32_t originAbilityInfoSize = static_cast<uint32_t>(originAbilityInfos.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, originAbilityInfoSize);

    for (uint32_t i = 0; i < originAbilityInfos.size(); ++i) {
        WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &originAbilityInfos[i]);
    }
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply,
        option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceQueryDomainTest001
 * @tc.desc: query verify status
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryDomainTest001, TestSize.Level0)
{
    std::string bundleName = "bundleName";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::QUERY_VERIFY_STATUS, data, reply,
        option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceQueryAllDomainTest001
 * @tc.desc: query verify status without bundle name
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryAllDomainTest001, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());

    int32_t  error = appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::QUERY_ALL_VERIFY_STATUS, data, reply,
        option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceSaveAllDomainTest001
 * @tc.desc: save all domain
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceSaveAllDomainTest001, TestSize.Level0)
{
    VerifyResultInfo verifyResultInfo;
    std::string bundleName = "bundleName";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &verifyResultInfo);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::SAVE_VERIFY_STATUS, data, reply,
        option);
    ASSERT_TRUE(error == ERR_OK);
}

/**
 * @tc.name: MgrServiceTest022
 * @tc.desc: dump null
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest022, TestSize.Level0)
{
    appDomainVerifyMgrService->OnDump();
    int fd = 0;
    std::vector<std::u16string> args;
    int ret = appDomainVerifyMgrService->Dump(fd, args);
    ASSERT_TRUE(ret == ERR_OK);
}
/**
 * @tc.name: MgrServiceTest023
 * @tc.desc: dump normal
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest023, TestSize.Level0)
{
    appDomainVerifyMgrService->OnDump();
    int fd = 0;
    std::vector<std::u16string> args;
    std::unordered_map<std::string, VerifyResultInfo> verifyMap;
    VerifyResultInfo verifyResultInfo;
    verifyMap.emplace("com.example", verifyResultInfo);
    appDomainVerifyMgrService->dataManager_->verifyMap_->swap(verifyMap);
    int ret = appDomainVerifyMgrService->Dump(fd, args);
    ASSERT_TRUE(ret == ERR_OK);
}
/**
 * @tc.name: MgrServiceTest024
 * @tc.desc: multi post DelayUnloadTask
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest024, TestSize.Level0)
{
    appDomainVerifyMgrService->PostDelayUnloadTask();
    appDomainVerifyMgrService->PostDelayUnloadTask();
}
}