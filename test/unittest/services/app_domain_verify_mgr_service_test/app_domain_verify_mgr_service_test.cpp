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
#include "mock_access_token.h"
#include "app_domain_verify_mgr_interface_code.h"
#include "rdb_helper.h"
#include "app_domain_verify_mgr_service_proxy.h"
#include "app_domain_verify_mgr_interface_code.h"
#include "system_ability_definition.h"
#include "app_domain_verify_parcel_util.h"
#include "mock_convert_callback.h"

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

int InvokeSingleVerifyOK(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "call end");
    std::string bundleName = BUNDLE_NAME;
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.insert_or_assign("https://" + HOST, InnerVerifyStatus::STATE_SUCCESS);
    appDomainVerifyMgrService->SaveDomainVerifyStatus(bundleName, verifyResultInfo);
    return 0;
}

int InvokeSingleVerifyFail(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_BUTT, "call end");
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
    MockAccessToken::mockSA();
    AppDomainVerifyAgentClient::staticDestoryMonitor_.destoryed_ = true;
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
 * @tc.desc: UpdateWhiteListUrls
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest024, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string url = "https://www.openharmonytest1.com";
    EXPECT_FALSE(appDomainVerifyMgrService->IsAtomicServiceUrl(url));

    std::vector<std::string> urls{ url };
    appDomainVerifyMgrService->UpdateWhiteListUrls(urls);
    EXPECT_TRUE(appDomainVerifyMgrService->IsAtomicServiceUrl(url));
}

/**
 * @tc.name: MgrServiceTest025
 * @tc.desc: UpdateWhiteListUrls
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest025, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string url2 = "https://www.openharmonytest2.com";
    std::string url3 = "https://www.openharmonytest2.com";
    EXPECT_FALSE(appDomainVerifyMgrService->IsAtomicServiceUrl(url2));
    EXPECT_FALSE(appDomainVerifyMgrService->IsAtomicServiceUrl(url3));

    std::vector<std::string> urls{ url2, url3};
    appDomainVerifyMgrService->UpdateWhiteListUrls(urls);
    EXPECT_TRUE(appDomainVerifyMgrService->IsAtomicServiceUrl(url3));
}

/**
 * @tc.name: MgrServiceTest026
 * @tc.desc: UpdateWhiteListUrls
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest026, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    std::string getDumpString{ " " };
    appDomainVerifyMgrService->DumpAllVerifyInfos(getDumpString);
    EXPECT_FALSE(getDumpString.empty());
}

/**
 * @tc.name: MgrServiceTest027
 * @tc.desc: IsWantImplicit
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest027, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    AAFwk::Want want;
    EXPECT_TRUE(appDomainVerifyMgrService->IsWantImplicit(want));
}

/**
 * @tc.name: MgrServiceTest028
 * @tc.desc: IsWantImplicit
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest028, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    AAFwk::Want want;
    AppExecFwk::ElementName element("10", "bundleName", "abilityName", "moduleName");
    want.SetElement(element);
    EXPECT_FALSE(appDomainVerifyMgrService->IsWantImplicit(want));
}

/**
 * @tc.name: MgrServiceTest029
 * @tc.desc: QueryDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest029, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    ASSERT_TRUE(appDomainVerifyMgrService->dataManager_);
    std::string bundleName{ "MgrServiceTest029" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(bundleName, InnerVerifyStatus::STATE_SUCCESS);
    verifyResultInfo.hostVerifyStatusMap.emplace("test", InnerVerifyStatus::STATE_SUCCESS);
    appDomainVerifyMgrService->dataManager_->SaveVerifyStatus(bundleName, verifyResultInfo);

    DomainVerifyStatus domainVerificationState = DomainVerifyStatus::STATE_NONE;
    EXPECT_TRUE(appDomainVerifyMgrService->QueryDomainVerifyStatus(bundleName, domainVerificationState));
    EXPECT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_VERIFIED);
}

/**
 * @tc.name: MgrServiceTest030
 * @tc.desc: QueryDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest030, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    ASSERT_TRUE(appDomainVerifyMgrService->dataManager_);
    std::string bundleName{ "MgrServiceTest030" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(bundleName, InnerVerifyStatus::UNKNOWN);
    appDomainVerifyMgrService->dataManager_->SaveVerifyStatus(bundleName, verifyResultInfo);

    DomainVerifyStatus domainVerificationState = DomainVerifyStatus::STATE_NONE;
    EXPECT_TRUE(appDomainVerifyMgrService->QueryDomainVerifyStatus(bundleName, domainVerificationState));
    EXPECT_TRUE(domainVerificationState != DomainVerifyStatus::STATE_VERIFIED);
}

/**
 * @tc.name: MgrServiceTest031
 * @tc.desc: QueryDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest031, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    ASSERT_TRUE(appDomainVerifyMgrService->dataManager_);
    std::string bundleName{ "MgrServiceTest031" };
    VerifyResultInfo verifyResultInfo;
    appDomainVerifyMgrService->dataManager_->SaveVerifyStatus(bundleName, verifyResultInfo);

    DomainVerifyStatus domainVerificationState = DomainVerifyStatus::STATE_NONE;
    EXPECT_TRUE(appDomainVerifyMgrService->QueryDomainVerifyStatus(bundleName, domainVerificationState));
    EXPECT_TRUE(domainVerificationState == DomainVerifyStatus::STATE_NONE);
}

/**
 * @tc.name: MgrServiceTest032
 * @tc.desc: QueryAllDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest032, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    ASSERT_TRUE(appDomainVerifyMgrService->dataManager_);
    std::string bundleName{ "MgrServiceTest031" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(bundleName, InnerVerifyStatus::STATE_FAIL);
    appDomainVerifyMgrService->dataManager_->SaveVerifyStatus(bundleName, verifyResultInfo);

    BundleVerifyStatusInfo bundleVerificationState;
    EXPECT_TRUE(appDomainVerifyMgrService->QueryAllDomainVerifyStatus(bundleVerificationState));
    EXPECT_TRUE(bundleVerificationState.bundleVerifyStatusInfoMap_.size() != 0);
}

/**
 * @tc.name: MgrServiceTest033
 * @tc.desc: SaveDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest033, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    ASSERT_TRUE(appDomainVerifyMgrService->dataManager_);
    std::string bundleName{ "MgrServiceTest033" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(bundleName, InnerVerifyStatus::STATE_FAIL);
    EXPECT_TRUE(appDomainVerifyMgrService->SaveDomainVerifyStatus(bundleName, verifyResultInfo));
    VerifyResultInfo getVerifyResultInfo;
    EXPECT_TRUE(appDomainVerifyMgrService->dataManager_->GetVerifyStatus(bundleName, getVerifyResultInfo));
}

/**
 * @tc.name: MgrServiceTest034
 * @tc.desc: SaveDomainVerifyStatus
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest034, TestSize.Level0)
{
    ASSERT_TRUE(appDomainVerifyMgrService);
    ASSERT_TRUE(appDomainVerifyMgrService->dataManager_);
    std::string bundleName{ "" };
    VerifyResultInfo verifyResultInfo;
    verifyResultInfo.hostVerifyStatusMap.emplace(bundleName, InnerVerifyStatus::STATE_FAIL);
    EXPECT_FALSE(appDomainVerifyMgrService->SaveDomainVerifyStatus(bundleName, verifyResultInfo));
}
}