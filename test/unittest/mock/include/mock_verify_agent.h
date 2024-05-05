/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef MOC_APPDOMAINVERIFY_AGENT_STUB_MOCK_H
#define MOC_APPDOMAINVERIFY_AGENT_STUB_MOCK_H
#include <gmock/gmock.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "i_app_domain_verify_agent_service.h"
#include "app_domain_verify_agent_service_proxy.h"
#include "app_domain_verify_agent_service_stub.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyAgentRemoteStubMock : public IRemoteStub<IAppDomainVerifyAgentService> {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"IAppDomainVerifyAgentServiceMock");
    AppDomainVerifyAgentRemoteStubMock()
    {
        printf("AppDomainVerifyAgentRemoteStubMock \n");
    };
    virtual ~AppDomainVerifyAgentRemoteStubMock()
    {
        printf("~AppDomainVerifyAgentRemoteStubMock \n");
    };
    virtual void SingleVerify(
        const AppVerifyBaseInfo& appVerifyBaseInfo, const std::vector<SkillUri>& skillUris) override
    {
    }
    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
    MOCK_METHOD(sptr<IRemoteObject>, AsObject, (), (override));
};

class AppDomainVerifyAgentStubMock : public AppDomainVerifyAgentServiceStub {
public:
    AppDomainVerifyAgentStubMock(){};
    virtual ~AppDomainVerifyAgentStubMock(){};
    virtual void SingleVerify(
        const AppVerifyBaseInfo& appVerifyBaseInfo, const std::vector<SkillUri>& skillUris) override
    {
    }
    virtual void ExitIdleState() override
    {
    }
};

}  // namespace AppDomainVerify
}  // namespace OHOS

#endif
