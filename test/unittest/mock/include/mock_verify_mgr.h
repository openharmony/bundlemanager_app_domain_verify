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

#ifndef MOC_APPDOMAINVERIFY_MGR_STUB_MOCK_H
#define MOC_APPDOMAINVERIFY_MGR_STUB_MOCK_H
#include <gmock/gmock.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "app_domain_verify_mgr_service_stub.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyMgrRemoteStubMock : public IRemoteStub<IAppDomainVerifyMgrService> {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"IAppDomainVerifyMgrServiceMock");
    AppDomainVerifyMgrRemoteStubMock(){};
    virtual ~AppDomainVerifyMgrRemoteStubMock(){};
    virtual void VerifyDomain(const std::string& appIdentifier, const std::string& bundleName,
        const std::string& fingerprint, const std::vector<SkillUri>& skillUris) override
    {
    }
    virtual bool ClearDomainVerifyStatus(const std::string& appIdentifier, const std::string& bundleName) override
    {
        return true;
    }
    virtual bool FilterAbilities(const OHOS::AAFwk::Want& want,
        const std::vector<OHOS::AppExecFwk::AbilityInfo>& originAbilityInfos,
        std::vector<OHOS::AppExecFwk::AbilityInfo>& filtedAbilityInfos) override
    {
        return true;
    }
    virtual bool QueryDomainVerifyStatus(
        const std::string& bundleName, DomainVerifyStatus& domainVerificationState) override
    {
        return true;
    }
    virtual bool QueryAllDomainVerifyStatus(BundleVerifyStatusInfo& bundleVerifyStatusInfo) override
    {
        return true;
    }
    virtual bool SaveDomainVerifyStatus(
        const std::string& bundleName, const VerifyResultInfo& verifyResultInfo) override
    {
        return true;
    }
    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
    MOCK_METHOD(sptr<IRemoteObject>, AsObject, (), (override));
};

class AppDomainVerifyMgrStubMock : public AppDomainVerifyMgrServiceStub {
public:
    AppDomainVerifyMgrStubMock(){};
    virtual ~AppDomainVerifyMgrStubMock(){};

    virtual void VerifyDomain(const std::string& appIdentifier, const std::string& bundleName,
        const std::string& fingerprint, const std::vector<SkillUri>& skillUris) override
    {
    }
    virtual bool ClearDomainVerifyStatus(const std::string& appIdentifier, const std::string& bundleName) override
    {
        return true;
    }
    virtual bool FilterAbilities(const OHOS::AAFwk::Want& want,
        const std::vector<OHOS::AppExecFwk::AbilityInfo>& originAbilityInfos,
        std::vector<OHOS::AppExecFwk::AbilityInfo>& filtedAbilityInfos) override
    {
        std::vector<OHOS::AppExecFwk::AbilityInfo> abilityInfos;
        filtedAbilityInfos = abilityInfos;
        return true;
    }
    virtual bool QueryDomainVerifyStatus(
        const std::string& bundleName, DomainVerifyStatus& domainVerificationState) override
    {
        return true;
    }
    virtual bool QueryAllDomainVerifyStatus(BundleVerifyStatusInfo& bundleVerifyStatusInfo) override
    {
        return true;
    }
    virtual bool SaveDomainVerifyStatus(
        const std::string& bundleName, const VerifyResultInfo& verifyResultInfo) override
    {
        return true;
    }
};
}  // namespace AppDomainVerify
}  // namespace OHOS

#endif
