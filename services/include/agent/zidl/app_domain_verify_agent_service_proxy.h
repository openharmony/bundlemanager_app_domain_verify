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

#ifndef APP_DOMAIN_VERIFY_AGENT_SERVICE_PROXY_H
#define APP_DOMAIN_VERIFY_AGENT_SERVICE_PROXY_H

#include "i_app_domain_verify_agent_service.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyAgentServiceProxy : public IRemoteProxy<IAppDomainVerifyAgentService> {
public:
    explicit AppDomainVerifyAgentServiceProxy(const sptr<IRemoteObject> &object);
    virtual ~AppDomainVerifyAgentServiceProxy();
    virtual void SingleVerify(const AppVerifyBaseInfo &appVerifyBaseInfo,
        const std::vector<SkillUri> &skillUris) override;

private:
    static inline BrokerDelegator<AppDomainVerifyAgentServiceProxy> delegator_;
};
}  // namespace AppDomainVerify
}  // namespace OHOS

#endif  // APP_DOMAIN_VERIFY_SERVICE_PROXY_H