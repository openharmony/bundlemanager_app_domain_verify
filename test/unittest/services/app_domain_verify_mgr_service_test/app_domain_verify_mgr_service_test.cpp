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

#include "app_domain_verify_rdb_data_manager.h"
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
#include "mock_access_token.h"
#include "app_domain_verify_mgr_interface_code.h"
#include "rdb_helper.h"
#include "app_domain_verify_mgr_service_proxy.h"
#include "app_domain_verify_mgr_interface_code.h"
#include "system_ability_definition.h"
#include "app_domain_verify_parcel_util.h"
#include "mock_convert_callback.h"
#include "permission_manager.h"
#include "mock_permission_mgr.h"
#include "mock_deferred_link_mgr.h"
#include "mock_bundle_manager.h"
#include "mock_system_ability.h"
#include "mock_system_ability_registry.h"

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
using namespace OHOS::AppExecFwk;
std::shared_ptr<AppDomainVerifyMgrService> appDomainVerifyMgrService = std::make_shared<AppDomainVerifyMgrService>();
auto appDomainVerifyAgentStubMock_ = std::make_shared<AppDomainVerifyAgentRemoteStubMock>();
class MgrServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

int InvokeSingleVerifyOK(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "call end");
    std::string bundleName = BUNDLE_NAME;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        "https://" + HOST, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    appDomainVerifyMgrService->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
    return 0;
}

int InvokeSingleVerifyFail(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "call end");
    std::string bundleName = BUNDLE_NAME;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        "https://" + HOST, std::make_tuple(InnerVerifyStatus::STATE_FAIL, std::string(), 0));
    appDomainVerifyMgrService->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
    return UNKNOWN_ERROR;
}
void MgrServiceTest::SetUpTestCase(void)
{
    g_mockBundleMgrService = new AppExecFwk::BundleMgrService();
    printf("SetUpTestCase \n");
}

void MgrServiceTest::TearDownTestCase(void)
{
    printf("TearDownTestCase  1\n");
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->rdbDataManager_ = nullptr;
    AppDomainVerifyAgentClient::agentServiceProxy_.ForceSetRefPtr(nullptr);
    printf("TearDownTestCase 2\n");
    AppDomainVerifyAgentClient::DestroyInstance();
    printf("TearDownTestCase 3\n");
    appDomainVerifyMgrService->appDetailsDataMgr_ = nullptr;
    appDomainVerifyMgrService->Stop();
    appDomainVerifyAgentStubMock_.reset();
    printf("TearDownTestCase \n");
}

void MgrServiceTest::SetUp(void)
{
    MockAccessToken::mockSA();
    AppDomainVerifyAgentClient::staticDestoryMonitor_.destoryed_ = true;
    printf("SetUp \n");
}

void MgrServiceTest::TearDown(void)
{
    DoMocPermissionManager(nullptr);
    DoMocDeferredLinkMgr(nullptr);
    g_mockBundleMgrService->impl = nullptr;
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
        .Times(AtLeast(1))
        .WillRepeatedly(::testing::Invoke(InvokeSingleVerifyOK));
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
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN, data, reply, option);
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

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN, data, reply, option);
    ASSERT_TRUE(error != ERR_OK);
}
/**
 * @tc.name: MgrServiceVerifyDomainTest003
 * @tc.desc: verify domain without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceVerifyDomainTest003, TestSize.Level0)
{
    EXPECT_CALL(*appDomainVerifyAgentStubMock_, SendRequest(_, _, _, _))
        .Times(AtLeast(1))
        .WillRepeatedly(::testing::Invoke(InvokeSingleVerifyOK));
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

    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceVerifyDomainTest004
 * @tc.desc: OnRemoteRequest with wrong opconde
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceVerifyDomainTest004, TestSize.Level0)
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
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::CLEAR_DOMAIN_VERIFY_RESULT, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceClearDomainTest002
 * @tc.desc: clear domain verify without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceClearDomainTest002, TestSize.Level0)
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
    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::CLEAR_DOMAIN_VERIFY_RESULT, data, reply, option);
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
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply, option);
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
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply, option);
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
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceFilterDomainTest004
 * @tc.desc: filter abilities without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceFilterDomainTest004, TestSize.Level0)
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
    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply, option);
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

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::QUERY_VERIFY_STATUS, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceQueryDomainTest002
 * @tc.desc: query verify status without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryDomainTest002, TestSize.Level0)
{
    std::string bundleName = "bundleName";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);

    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);
    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::QUERY_VERIFY_STATUS, data, reply, option);
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

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::QUERY_ALL_VERIFY_STATUS, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceQueryAllDomainTest002
 * @tc.desc: query verify status without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryAllDomainTest002, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());

    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::QUERY_ALL_VERIFY_STATUS, data, reply, option);
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

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::SAVE_VERIFY_STATUS, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceSaveAllDomainTest002
 * @tc.desc: save all domain without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceSaveAllDomainTest002, TestSize.Level0)
{
    VerifyResultInfo verifyResultInfo;
    std::string bundleName = "bundleName";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &verifyResultInfo);

    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::SAVE_VERIFY_STATUS, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceIsAtomicUrlTest001
 * @tc.desc: is aotmic service url
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceIsAtomicUrlTest001, TestSize.Level0)
{
    std::string url = "https://www.openharmony.com";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IAppDomainVerifyMgrService::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, url);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::IS_ATOMIC_SERVICE_URL, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceIsAtomicUrlTest002
 * @tc.desc: is aotmic service url without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceIsAtomicUrlTest002, TestSize.Level0)
{
    std::string url = "https://www.openharmony.com";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IAppDomainVerifyMgrService::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, url);

    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::IS_ATOMIC_SERVICE_URL, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceConvertToExplicitWantTest001
 * @tc.desc: convert to explicit want
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceConvertToExplicitWantTest001, TestSize.Level0)
{
    OHOS::AAFwk::Want implicitWant;
    sptr<MocConvertCallback> callback = new MocConvertCallback;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IAppDomainVerifyMgrService::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &implicitWant);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(RemoteObject, data, callback->AsObject());

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::CONVERT_TO_EXPLICIT_WANT, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}

/**
 * @tc.name: MgrServiceConvertToExplicitWantTest002
 * @tc.desc: convert to explicit want with out want
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceConvertToExplicitWantTest002, TestSize.Level0)
{
    OHOS::AAFwk::Want implicitWant;
    sptr<MocConvertCallback> callback = new MocConvertCallback;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IAppDomainVerifyMgrService::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(RemoteObject, data, callback->AsObject());

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::CONVERT_TO_EXPLICIT_WANT, data, reply, option);
    ASSERT_TRUE(error != ERR_OK);
}
/**
 * @tc.name: MgrServiceConvertToExplicitWantTest003
 * @tc.desc: convert to explicit want without cb
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceConvertToExplicitWantTest003, TestSize.Level0)
{
    OHOS::AAFwk::Want implicitWant;
    sptr<MocConvertCallback> callback = new MocConvertCallback;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IAppDomainVerifyMgrService::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &implicitWant);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::CONVERT_TO_EXPLICIT_WANT, data, reply, option);
    ASSERT_TRUE(error != ERR_OK);
}
/**
 * @tc.name: MgrServiceConvertToExplicitWantTest004
 * @tc.desc: convert to explicit want without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceConvertToExplicitWantTest004, TestSize.Level0)
{
    OHOS::AAFwk::Want implicitWant;
    sptr<MocConvertCallback> callback = new MocConvertCallback;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IAppDomainVerifyMgrService::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &implicitWant);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(RemoteObject, data, callback->AsObject());

    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::CONVERT_TO_EXPLICIT_WANT, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceDumpTest001
 * @tc.desc: dump null
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceDumpTest001, TestSize.Level0)
{
    appDomainVerifyMgrService->OnDump();
    int fd = 0;
    std::vector<std::u16string> args;
    int ret = appDomainVerifyMgrService->Dump(fd, args);
    ASSERT_TRUE(ret == ERR_OK);
}
/**
 * @tc.name: MgrServiceDumpTest002
 * @tc.desc: dump normal
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceDumpTest002, TestSize.Level0)
{
    appDomainVerifyMgrService->OnDump();
    int fd = 0;
    std::vector<std::u16string> args;
    std::unordered_map<std::string, VerifyResultInfo> verifyMap;
    VerifyResultInfo verifyResultInfo;
    verifyMap.emplace("com.example", verifyResultInfo);
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->verifyMap_->swap(verifyMap);
    int ret = appDomainVerifyMgrService->Dump(fd, args);
    ASSERT_TRUE(ret == ERR_OK);
}
/**
 * @tc.name: MgrServiceIsAtomicServiceUrlTest001
 * @tc.desc: IsAtomicServiceUrl
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceIsAtomicServiceUrlTest001, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string url = "https://www.openharmonytest1.com";
    EXPECT_FALSE(appDomainVerifyMgrService->IsAtomicServiceUrl(url));

    std::vector<std::string> urls{ url };
    appDomainVerifyMgrService->UpdateWhiteListUrls(urls);
    EXPECT_TRUE(appDomainVerifyMgrService->IsAtomicServiceUrl(url));
}

/**
 * @tc.name: MgrServiceIsAtomicServiceUrlTest002
 * @tc.desc: IsAtomicServiceUrl
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceIsAtomicServiceUrlTest002, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string url2 = "https://www.openharmonytest2.com";
    std::string url3 = "https://www.openharmonytest2.com";
    EXPECT_FALSE(appDomainVerifyMgrService->IsAtomicServiceUrl(url2));
    EXPECT_FALSE(appDomainVerifyMgrService->IsAtomicServiceUrl(url3));

    std::vector<std::string> urls{ url2, url3 };
    appDomainVerifyMgrService->UpdateWhiteListUrls(urls);
    EXPECT_TRUE(appDomainVerifyMgrService->IsAtomicServiceUrl(url3));
}

/**
 * @tc.name: MgrServiceDumpAllVerifyInfosTest001
 * @tc.desc: DumpAllVerifyInfos
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceDumpAllVerifyInfosTest001, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string getDumpString{ " " };
    appDomainVerifyMgrService->DumpAllVerifyInfos(getDumpString);
    EXPECT_FALSE(getDumpString.empty());
}

/**
 * @tc.name: MgrServiceDumpAllVerifyInfosTest002
 * @tc.desc: DumpAllVerifyInfos without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceDumpAllVerifyInfosTest002, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string getDumpString{ " " };

    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);

    appDomainVerifyMgrService->DumpAllVerifyInfos(getDumpString);
    EXPECT_FALSE(getDumpString.empty());
}

/**
 * @tc.name: MgrServiceIsWantImplicitTest001
 * @tc.desc: IsWantImplicit
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceIsWantImplicitTest001, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    AAFwk::Want want;
    EXPECT_TRUE(appDomainVerifyMgrService->IsWantImplicit(want));
}

/**
 * @tc.name: MgrServiceIsWantImplicitTest002
 * @tc.desc: IsWantImplicit
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceIsWantImplicitTest002, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    AAFwk::Want want;
    AppExecFwk::ElementName element("10", "bundleName", "abilityName", "moduleName");
    want.SetElement(element);
    EXPECT_FALSE(appDomainVerifyMgrService->IsWantImplicit(want));

    AAFwk::Want want1;
    AppExecFwk::ElementName element1("10", "", "", "moduleName");
    want.SetElement(element);
    EXPECT_TRUE(appDomainVerifyMgrService->IsWantImplicit(want1));
}

/**
 * @tc.name: MgrServiceQueryDomainVerifyStatusTest001
 * @tc.desc: QueryDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryDomainVerifyStatusTest001, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string bundleName{ "MgrServiceTest029" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(
        bundleName, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    verifyResultInfo.hostVerifyStatusMap.emplace(
        "test", std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(bundleName, verifyResultInfo);

    DomainVerifyStatus domainVerificationState = DomainVerifyStatus::STATE_NONE;
    EXPECT_TRUE(appDomainVerifyMgrService->QueryDomainVerifyStatus(bundleName, domainVerificationState));
    EXPECT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_VERIFIED);
}

/**
 * @tc.name: MgrServiceQueryDomainVerifyStatusTest002
 * @tc.desc: QueryDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryDomainVerifyStatusTest002, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string bundleName{ "MgrServiceTest030" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(
        bundleName, std::make_tuple(InnerVerifyStatus::UNKNOWN, std::string(), 0));
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(bundleName, verifyResultInfo);
    DomainVerifyStatus domainVerificationState = DomainVerifyStatus::STATE_NONE;
    EXPECT_TRUE(appDomainVerifyMgrService->QueryDomainVerifyStatus(bundleName, domainVerificationState));
    EXPECT_TRUE(domainVerificationState != DomainVerifyStatus::STATE_VERIFIED);
}

/**
 * @tc.name: MgrServiceQueryDomainVerifyStatusTest003
 * @tc.desc: QueryDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryDomainVerifyStatusTest003, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string bundleName{ "MgrServiceTest031" };
    VerifyResultInfo verifyResultInfo;
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(bundleName, verifyResultInfo);

    DomainVerifyStatus domainVerificationState = DomainVerifyStatus::STATE_NONE;
    EXPECT_TRUE(appDomainVerifyMgrService->QueryDomainVerifyStatus(bundleName, domainVerificationState));
    EXPECT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}

/**
 * @tc.name: MgrServiceQueryDomainVerifyStatusTest004
 * @tc.desc: QueryAllDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryDomainVerifyStatusTest004, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string bundleName{ "MgrServiceTest031" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(
        bundleName, std::make_tuple(InnerVerifyStatus::STATE_FAIL, std::string(), 0));
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(bundleName, verifyResultInfo);

    BundleVerifyStatusInfo bundleVerificationState;
    EXPECT_TRUE(appDomainVerifyMgrService->QueryAllDomainVerifyStatus(bundleVerificationState));
    EXPECT_TRUE(bundleVerificationState.bundleVerifyStatusInfoMap_.size() != 0);
}
/**
 * @tc.name: MgrServiceQueryDomainVerifyStatusTest005
 * @tc.desc: QueryDomainVerifyStatus without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryDomainVerifyStatusTest005, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string bundleName{ "MgrServiceTest029" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(
        bundleName, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    verifyResultInfo.hostVerifyStatusMap.emplace(
        "test", std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(bundleName, verifyResultInfo);

    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);

    DomainVerifyStatus domainVerificationState = DomainVerifyStatus::STATE_NONE;
    EXPECT_FALSE(appDomainVerifyMgrService->QueryDomainVerifyStatus(bundleName, domainVerificationState));
}
/**
 * @tc.name: MgrServiceSaveDomainVerifyStatusTest001
 * @tc.desc: SaveDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceSaveDomainVerifyStatusTest001, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string bundleName{ "MgrServiceTest033" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(
        bundleName, std::make_tuple(InnerVerifyStatus::STATE_FAIL, std::string(), 0));
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(bundleName, verifyResultInfo);
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        bundleName, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    EXPECT_TRUE(appDomainVerifyMgrService->SaveDomainVerifyStatus(bundleName, verifyResultInfo));
    VerifyResultInfo getVerifyResultInfo;
    auto dataMgr = DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance();
    EXPECT_TRUE(dataMgr->GetVerifyStatus(bundleName, getVerifyResultInfo));
}

/**
 * @tc.name: MgrServiceSaveDomainVerifyStatusTest002
 * @tc.desc: SaveDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceSaveDomainVerifyStatusTest002, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string bundleName{ "" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(
        bundleName, std::make_tuple(InnerVerifyStatus::STATE_FAIL, std::string(), 0));
    EXPECT_FALSE(appDomainVerifyMgrService->SaveDomainVerifyStatus(bundleName, verifyResultInfo));
}
/**
 * @tc.name: MgrServiceSaveDomainVerifyStatusTest003
 * @tc.desc: SaveDomainVerifyStatus without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceSaveDomainVerifyStatusTest003, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string bundleName{ "MgrServiceTest033" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(
        bundleName, std::make_tuple(InnerVerifyStatus::STATE_FAIL, std::string(), 0));
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(bundleName, verifyResultInfo);
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        bundleName, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    EXPECT_TRUE(appDomainVerifyMgrService->SaveDomainVerifyStatus(bundleName, verifyResultInfo));
}

/**
 * @tc.name: QueryAssociatedDomains001
 * @tc.desc: QueryAssociatedDomains ok
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, QueryAssociatedDomains001, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        BUNDLE_NAME, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(BUNDLE_NAME, verifyResultInfo);
    std::vector<std::string> domains;
    EXPECT_EQ(appDomainVerifyMgrService->QueryAssociatedDomains(BUNDLE_NAME, domains), ErrorCode::E_OK);
}
/**
 * @tc.name: QueryAssociatedDomains002
 * @tc.desc: QueryAssociatedDomains without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, QueryAssociatedDomains002, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string bundleName;
    std::vector<std::string> domains;
    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, CheckPermission(_)).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);
    EXPECT_EQ(appDomainVerifyMgrService->QueryAssociatedDomains(bundleName, domains), 201);
}
/**
 * @tc.name:QueryAssociatedBundleNames001
 * @tc.desc: QueryAssociatedBundleNames
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, QueryAssociatedBundleNames001, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string domain;
    std::vector<std::string> bundleNames;
    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, CheckPermission(_)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mocPermissionManager, IsSystemAppCall()).Times(1).WillOnce(Return(true));
    DoMocPermissionManager(mocPermissionManager);
    EXPECT_EQ(appDomainVerifyMgrService->QueryAssociatedBundleNames(domain, bundleNames), 0);
}

/**
 * @tc.name:QueryAssociatedBundleNames002
 * @tc.desc: QueryAssociatedBundleNames without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, QueryAssociatedBundleNames002, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string domain;
    std::vector<std::string> bundleNames;
    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, CheckPermission(_)).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);
    EXPECT_EQ(appDomainVerifyMgrService->QueryAssociatedBundleNames(domain, bundleNames), 201);
}
/**
 * @tc.name:QueryAssociatedBundleNames003
 * @tc.desc: QueryAssociatedBundleNames without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, QueryAssociatedBundleNames003, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string domain;
    std::vector<std::string> bundleNames;
    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, CheckPermission(_)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mocPermissionManager, IsSystemAppCall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);
    EXPECT_EQ(appDomainVerifyMgrService->QueryAssociatedBundleNames(domain, bundleNames), 202);
}
/**
 * @tc.name: OnUpdateWhiteListUrls001
 * @tc.desc: OnUpdateWhiteListUrls
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, OnUpdateWhiteListUrls001, TestSize.Level0)
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

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::UPDATE_WHITE_LIST_URLS, data, reply, option);
    ASSERT_TRUE(error != ERR_OK);
}
/**
 * @tc.name: OnUpdateWhiteListUrls002
 * @tc.desc: OnUpdateWhiteListUrls without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, OnUpdateWhiteListUrls002, TestSize.Level0)
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
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IAppDomainVerifyMgrService::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, 0);

    uint32_t size = static_cast<uint32_t>(skillUris.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, size);

    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::UPDATE_WHITE_LIST_URLS, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}

bool InvokeGetBundleNameForUid(const int uid, std::string& bundleName)
{
    bundleName = BUNDLE_NAME;
    return true;
}
ErrCode InvokeGetBundleInfoV9(const std::string& bundleName, int32_t flags, BundleInfo& bundleInfo, int32_t userId)
{
    bundleInfo.signatureInfo.appIdentifier = AppDomainVerify::APP_IDENTIFIER;
    bundleInfo.signatureInfo.fingerprint = AppDomainVerify::FINGERPRINT;
    return 0;
}

/**
 * @tc.name: GetDeferredLink001
 * @tc.desc: GetDeferredLink
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, GetDeferredLink001, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IAppDomainVerifyMgrService::GetDescriptor());

    // can not get bundleName
    auto mocBundleMgrService = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService, GetBundleNameForUid(_, _)).WillOnce(Return(false));
    g_mockBundleMgrService->impl = mocBundleMgrService;

    std::string link;
    int32_t error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error != ERR_OK);

    // get empty bundleName
    auto mocBundleMgrService1 = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService1, GetBundleNameForUid(_, _)).WillOnce(Return(true));
    g_mockBundleMgrService->impl = mocBundleMgrService1;

    error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error != ERR_OK);

    // can not get bundleInfo
    auto mocBundleMgrService2 = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService2, GetBundleNameForUid(_, _)).WillRepeatedly(InvokeGetBundleNameForUid);
    EXPECT_CALL(*mocBundleMgrService2, GetBundleInfoV9(_, _, _, _)).WillOnce(Return(false));
    g_mockBundleMgrService->impl = mocBundleMgrService2;

    error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error != ERR_OK);

    // empty app identifier
    auto mocBundleMgrService3 = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService3, GetBundleNameForUid(_, _)).WillRepeatedly(InvokeGetBundleNameForUid);
    EXPECT_CALL(*mocBundleMgrService3, GetBundleInfoV9(_, _, _, _)).WillOnce(Return(true));
    g_mockBundleMgrService->impl = mocBundleMgrService3;

    error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error != ERR_OK);

    // can not get verify result
    auto mocBundleMgrService4 = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService4, GetBundleNameForUid(_, _)).WillRepeatedly(InvokeGetBundleNameForUid);
    EXPECT_CALL(*mocBundleMgrService4, GetBundleInfoV9(_, _, _, _)).WillRepeatedly(InvokeGetBundleInfoV9);
    g_mockBundleMgrService->impl = mocBundleMgrService4;

    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->DeleteVerifyStatus(BUNDLE_NAME);
    error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error != ERR_OK);

    // app identifier not equal
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.appIdentifier = "";

    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->DeleteVerifyStatus(BUNDLE_NAME);
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(BUNDLE_NAME, verifyResultInfo);
    error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error != ERR_OK);

    // empty host status map
    verifyResultInfo.appIdentifier = AppDomainVerify::APP_IDENTIFIER;
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->DeleteVerifyStatus(BUNDLE_NAME);
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(BUNDLE_NAME, verifyResultInfo);
    error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error == ERR_OK);

    // empty success empty status map
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        BUNDLE_NAME, std::make_tuple(InnerVerifyStatus::STATE_FAIL, std::string(), 0));
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->DeleteVerifyStatus(BUNDLE_NAME);
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(BUNDLE_NAME, verifyResultInfo);
    error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error == ERR_OK);

    // empty success empty status map
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        BUNDLE_NAME, std::make_tuple(InnerVerifyStatus::STATE_FAIL, std::string(), 0));
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        BUNDLE_NAME + "1", std::make_tuple(InnerVerifyStatus::STATE_FAIL, std::string(), 0));
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->DeleteVerifyStatus(BUNDLE_NAME);
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(BUNDLE_NAME, verifyResultInfo);
    error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error == ERR_OK);

    // 1 success empty status map
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        BUNDLE_NAME, std::make_tuple(InnerVerifyStatus::STATE_SUCCESS, std::string(), 0));
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign(
        BUNDLE_NAME + "1", std::make_tuple(InnerVerifyStatus::STATE_FAIL, std::string(), 0));
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->DeleteVerifyStatus(BUNDLE_NAME);
    DelayedSingleton<AppDomainVerifyDataMgr>::GetInstance()->InsertVerifyStatus(BUNDLE_NAME, verifyResultInfo);
    error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error == ERR_OK);

    auto mocDeferredLinkMgr = std::make_shared<MocDeferredLinkMgr>();
    EXPECT_CALL(*mocDeferredLinkMgr, GetDeferredLink(_, _)).Times(1).WillOnce(Return(""));
    DoMocDeferredLinkMgr(mocDeferredLinkMgr);
    error = appDomainVerifyMgrService->GetDeferredLink(link);
    ASSERT_TRUE(error == ERR_OK);
}

/**
 * @tc.name: QueryAppDetailsWant001
 * @tc.desc: QueryAppDetailsWant
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, QueryAppDetailsWant001, TestSize.Level0)
{
    AAFwk::Want want;
    std::string url = "";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IAppDomainVerifyMgrService::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, url);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &want);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::QUERY_APP_DETAILS_WANT, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}

/**
 * @tc.name: QueryAppDetailsWant002
 * @tc.desc: QueryAppDetailsWant without permission
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, QueryAppDetailsWant002, TestSize.Level0)
{
    AAFwk::Want want;
    std::string url = "";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IAppDomainVerifyMgrService::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, url);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &want);

    auto mocPermissionManager = std::make_shared<MocPermissionManager>();
    EXPECT_CALL(*mocPermissionManager, IsSACall()).Times(1).WillOnce(Return(false));
    DoMocPermissionManager(mocPermissionManager);

    int32_t error = appDomainVerifyMgrService->OnRemoteRequest(
        AppDomainVerifyMgrInterfaceCode::QUERY_APP_DETAILS_WANT, data, reply, option);
    ASSERT_TRUE(error == ERR_OK);
}

/**
 * @tc.name: LifeCycle001
 * @tc.desc: MgrService lifecycle test
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, LifeCycle001, TestSize.Level0)
{
    auto service = std::make_shared<AppDomainVerifyMgrService>();
    service->OnStart();
    service->OnStop();
    ASSERT_TRUE(service != nullptr);
}

}