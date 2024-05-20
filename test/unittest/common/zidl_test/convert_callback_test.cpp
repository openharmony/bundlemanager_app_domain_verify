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
#define private public
#define protected public
#include "convert_callback_stub.h"
#include "convert_callback_proxy.h"
#undef private
#undef protected
#include "convert_callback_interface_code.h"
#include "parcel_util.h"
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
    void OnConvert(int resCode, AAFwk::Want& want) override
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
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Int32, data, resCode);
    WRITE_PARCEL_AND_RETURN_IF_FAIL(Parcelable, data, &want);
    sptr<CallBack> callback = new CallBack;
    callback->OnRemoteRequest(static_cast<int32_t>(ConvertCallbackInterfaceCode::ON_CONVERT_CALLBACK), data, reply, option);
}
/**
 * @tc.name: AppDomainVerifyConvertCallbackProxyTest001
 * @tc.desc: AddTask test
 * @tc.type: FUNC
 */
HWTEST_F(AppDomainVerifyConvertCallbackTest, AppDomainVerifyConvertCallbackProxyTest001, TestSize.Level0)
{
    sptr<ConvertCallbackProxy> proxy = new ConvertCallbackProxy(nullptr);
    int resCode = 0;
    OHOS::AAFwk::Want want;
    proxy->OnConvert(resCode, want);
}
}