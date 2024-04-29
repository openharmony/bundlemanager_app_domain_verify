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

#ifndef APP_DOMAIN_VERIFY_AGENT_SERVICE_STUB_H
#define APP_DOMAIN_VERIFY_AGENT_SERVICE_STUB_H
#include <map>
#include "i_app_domain_verify_agent_service.h"
#include "iremote_stub.h"
#include "event_handler.h"
#include "event_runner.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyAgentServiceStub : public IRemoteStub<IAppDomainVerifyAgentService> {
public:
    API_EXPORT AppDomainVerifyAgentServiceStub();
    API_EXPORT virtual ~AppDomainVerifyAgentServiceStub();
    API_EXPORT int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
    void PostDelayUnloadTask();

protected:
    virtual void ExitIdleState() = 0;

private:
    using ServiceFunc = int32_t (AppDomainVerifyAgentServiceStub::*)(MessageParcel &data, MessageParcel &reply);
    int32_t OnSingleVerify(MessageParcel &data, MessageParcel &reply);

private:
    std::map<uint32_t, ServiceFunc> memberFuncMap_;
    std::shared_ptr<AppExecFwk::EventHandler> unloadHandler_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
};
}  // namespace AppDomainVerify
}  // namespace OHOS

#endif  // APP_DOMAIN_VERIFY_SERVICE_STUB_H