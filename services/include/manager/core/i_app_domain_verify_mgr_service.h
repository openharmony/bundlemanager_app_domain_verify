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
#ifndef I_APP_DOMAIN_VERIFY_MGR_SERVICE_H
#define I_APP_DOMAIN_VERIFY_MGR_SERVICE_H
#include <string>

#include "app_domain_verify_hilog.h"
#include "iremote_broker.h"
#include "skill_uri.h"
#include "want.h"
#include "ability_info.h"
#include "domain_verify_status.h"
#include "bundle_verify_status_info.h"

namespace OHOS {
namespace AppDomainVerify {
class IAppDomainVerifyMgrService : public IRemoteBroker {
public:
    virtual void VerifyDomain(const std::string &appIdentifier, const std::string &bundleName,
        const std::string &fingerprint, const std::vector<SkillUri> &skillUris) = 0;
    virtual bool ClearDomainVerifyStatus(const std::string &appIdentifier, const std::string &bundleName) = 0;
    virtual bool FilterAbilities(const OHOS::AAFwk::Want &want,
        const std::vector<OHOS::AppExecFwk::AbilityInfo> &originAbilityInfos,
        std::vector<OHOS::AppExecFwk::AbilityInfo> &filtedAbilityInfos) = 0;
    virtual bool QueryDomainVerifyStatus(const std::string &bundleName,
        DomainVerifyStatus &domainVerificationState) = 0;
    virtual bool QueryAllDomainVerifyStatus(BundleVerifyStatusInfo &bundleVerifyStatusInfo) = 0;
    virtual bool SaveDomainVerifyStatus(const std::string &bundleName, const VerifyResultInfo &verifyResultInfo) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appDomainVerify.IAppDomainVerifyMgrService");
};
}  // namespace AppDomainVerify
}  // namespace OHOS

#endif