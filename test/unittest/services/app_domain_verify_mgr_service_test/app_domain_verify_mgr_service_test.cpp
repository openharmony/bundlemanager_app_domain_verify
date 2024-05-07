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
class MgrServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void MgrServiceTest::SetUpTestCase(void)
{
    printf("SetUpTestCase \n");
}

void MgrServiceTest::TearDownTestCase(void)
{
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
 * @tc.name: MgrServiceTest001
 * @tc.desc: ClearDomainVerifyStatus test
 * @tc.type: FUNC
*/
HWTEST_F(MgrServiceTest, MgrServiceTest001, TestSize.Level0)
{
    appDomainVerifyMgrService->OnStart();
    ASSERT_TRUE(appDomainVerifyMgrService->ClearDomainVerifyStatus(APP_IDENTIFIER, BUNDLE_NAME));
    appDomainVerifyMgrService->OnStop();
}

/**
 * @tc.name: MgrServiceTest002
 * @tc.desc: ClearDomainVerifyStatus test
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest002, TestSize.Level0)
{
    appDomainVerifyMgrService->OnStart();
    ASSERT_TRUE(appDomainVerifyMgrService->ClearDomainVerifyStatus(APP_IDENTIFIER, BUNDLE_NAME));
    appDomainVerifyMgrService->OnStop();
}

/**
 * @tc.name: MgrServiceVerifyDomainTest001
 * @tc.desc: Normal
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceVerifyDomainTest001, TestSize.Level0)
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

    for (uint32_t i = 0; i < skillUris.size(); ++i) {
        WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &skillUris[i]);
    }
    (void)appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN, data, reply, option);
}

/**
 * @tc.name: MgrServiceVerifyDomainTest002
 * @tc.desc: Normal
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceVerifyDomainTest002, TestSize.Level0)
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
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, std::u16string(u"abc"));
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, appIdentifier);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, fingerprint);

    uint32_t size = static_cast<uint32_t>(skillUris.size());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Uint32, data, size);

    (void)appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::VERIFY_DOMAIN, data, reply, option);
}

/**
 * @tc.name: MgrServiceVerifyDomainTest003
 * @tc.desc: Normal
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

    (void)appDomainVerifyMgrService->OnRemoteRequest(100, data, reply, option);
}
/**
 * @tc.name: MgrServiceClearDomainTest001
 * @tc.desc: Normal
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
    (void)appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::CLEAR_DOMAIN_VERIFY_RESULT, data, reply,
        option);

}

/**
 * @tc.name: MgrServiceFilterDomainTest001
 * @tc.desc: Normal
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
    (void)appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply,
        option);
}

/**
 * @tc.name: MgrServiceFilterDomainTest002
 * @tc.desc: Normal
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
    (void)appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply,
        option);
}

/**
 * @tc.name: MgrServiceFilterDomainTest003
 * @tc.desc: Normal
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
    (void)appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::FILTER_ABILITIES, data, reply,
        option);
}
/**
 * @tc.name: MgrServiceQueryDomainTest001
 * @tc.desc: Normal
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryDomainTest001, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    std::string bundleName = "bundleName";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);

    (void)appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::QUERY_VERIFY_STATUS, data, reply,
        option);
}
/**
 * @tc.name: MgrServiceQueryAllDomainTest001
 * @tc.desc: Normal
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceQueryAllDomainTest001, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());

    (void)appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::QUERY_ALL_VERIFY_STATUS, data, reply,
        option);
//    ASSERT_TRUE(error == ERR_OK);
}
/**
 * @tc.name: MgrServiceSaveAllDomainTest001
 * @tc.desc: Normal
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceSaveAllDomainTest001, TestSize.Level0)
{
    auto appDomainVerifyMgrStubMock = std::make_shared<AppDomainVerifyMgrStubMock>();
    VerifyResultInfo verifyResultInfo;
    std::string bundleName = "bundleName";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, AppDomainVerifyMgrServiceProxy::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(String, data, bundleName);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &verifyResultInfo);

    (void)appDomainVerifyMgrService->OnRemoteRequest(AppDomainVerifyMgrInterfaceCode::SAVE_VERIFY_STATUS, data, reply,
        option);

}

/**
 * @tc.name: MgrServiceTest022
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest022, TestSize.Level0)
{
    appDomainVerifyMgrService->OnDump();
    int fd = 0;
    std::vector<std::u16string> args;
    appDomainVerifyMgrService->Dump(fd, args);
}
/**
 * @tc.name: MgrServiceTest023
 * @tc.desc:
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
    printf("MgrServiceTest023 1\n");
    appDomainVerifyMgrService->dataManager_->verifyMap_->swap(verifyMap);
    printf("MgrServiceTest023 2\n");
    appDomainVerifyMgrService->Dump(fd, args);
    printf("MgrServiceTest023 3\n");
}
/**
 * @tc.name: MgrServiceTest024
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(MgrServiceTest, MgrServiceTest024, TestSize.Level0)
{
    appDomainVerifyMgrService->PostDelayUnloadTask();
    appDomainVerifyMgrService->PostDelayUnloadTask();
}

}