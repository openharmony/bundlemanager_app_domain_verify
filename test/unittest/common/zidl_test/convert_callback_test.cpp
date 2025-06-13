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
#include "convert_callback_stub.h"
#include "convert_callback_proxy.h"
#include "convert_callback_interface_code.h"
#include "app_domain_verify_parcel_util.h"
#include "target_info.h"
namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;
class AppDomainVerifyConvertCallbackTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AppDomainVerifyConvertCallbackTest::SetUpTestCase(void)
{
}

void AppDomainVerifyConvertCallbackTest::TearDownTestCase(void)
{
}

void AppDomainVerifyConvertCallbackTest::SetUp(void)
{
}

void AppDomainVerifyConvertCallbackTest::TearDown(void)
{
}
class CallBack : public ConvertCallbackStub {
public:
    void OnConvert(int resCode, TargetInfo& targetInfo) override
    {
    }
};

/**
 * @tc.name: AppDomainVerifyConvertCallbackStubTest001
 * @tc.desc: AddTask test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConvertCallbackTest, AppDomainVerifyConvertCallbackStubTest001, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    int resCode = 0;
    OHOS::AAFwk::Want want;
    TargetInfo targetInfo;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IConvertCallback::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Int32, data, resCode);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &targetInfo);
    sptr<CallBack> callback = new CallBack;
    auto ret = callback->OnRemoteRequest(
        static_cast<int32_t>(ConvertCallbackInterfaceCode::ON_CONVERT_CALLBACK), data, reply, option);
    ASSERT_TRUE(ret == 0);
}
/**
 * @tc.name: AppDomainVerifyConvertCallbackStubTest002
 * @tc.desc: AddTask test without description
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConvertCallbackTest, AppDomainVerifyConvertCallbackStubTest002, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    int resCode = 0;
    OHOS::AAFwk::Want want;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Int32, data, resCode);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &want);
    sptr<CallBack> callback = new CallBack;
    auto ret = callback->OnRemoteRequest(
        static_cast<int32_t>(ConvertCallbackInterfaceCode::ON_CONVERT_CALLBACK), data, reply, option);
    ASSERT_TRUE(ret != 0);
}
/**
 * @tc.name: AppDomainVerifyConvertCallbackStubTest003
 * @tc.desc: AddTask test with wrong code
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConvertCallbackTest, AppDomainVerifyConvertCallbackStubTest003, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    int resCode = 0;
    OHOS::AAFwk::Want want;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IConvertCallback::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Int32, data, resCode);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &want);
    sptr<CallBack> callback = new CallBack;
    auto ret = callback->OnRemoteRequest(100, data, reply, option);
    ASSERT_TRUE(ret != 0);
}
/**
 * @tc.name: AppDomainVerifyConvertCallbackStubTest004
 * @tc.desc: AddTask test with without want
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConvertCallbackTest, AppDomainVerifyConvertCallbackStubTest004, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    int resCode = 0;
    OHOS::AAFwk::Want want;
    WRITE_PARCEL_AND_RETURN_IF_FAIL(InterfaceToken, data, IConvertCallback::GetDescriptor());
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Int32, data, resCode);
    sptr<CallBack> callback = new CallBack;
    auto ret = callback->OnRemoteRequest(
        static_cast<int32_t>(ConvertCallbackInterfaceCode::ON_CONVERT_CALLBACK), data, reply, option);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: AppDomainVerifyTargetInfoTest001
 * @tc.desc: AddTask test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConvertCallbackTest, AppDomainVerifyTargetInfoTest001, TestSize.Level0)
{
    TargetInfo targetInfo;
    OHOS::AAFwk::Want want;
    want.SetUri("hello");
    targetInfo.targetType = TargetType::ATOMIC_SERVICE;
    targetInfo.targetWant = want;

    Parcel parcel;
    targetInfo.Marshalling(parcel);
    auto unmarshalling = TargetInfo::Unmarshalling(parcel);
    ASSERT_TRUE(unmarshalling->targetType == TargetType::ATOMIC_SERVICE);
    ASSERT_TRUE(unmarshalling->targetWant.GetUri() == Uri("hello"));

    Parcel parcel1;
    unmarshalling = TargetInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
    parcel1.WriteInt32(TargetType::ATOMIC_SERVICE);
    unmarshalling = TargetInfo::Unmarshalling(parcel1);
    ASSERT_TRUE(unmarshalling == nullptr);
}
}