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

#ifndef APP_DOMAIN_VERIFY_AGENT_CLIENT_H
#define APP_DOMAIN_VERIFY_AGENT_CLIENT_H

#include "singleton.h"
#include "i_app_domain_verify_agent_service.h"

namespace OHOS {
namespace AppDomainVerify {
class AppDomainVerifyAgentClient : public DelayedSingleton<AppDomainVerifyAgentClient> {
    DECLARE_DELAYED_SINGLETON(AppDomainVerifyAgentClient);

public:
    DISALLOW_COPY_AND_MOVE(AppDomainVerifyAgentClient);

    /**
     * SingleVerify
     * @descrition refresh single bundle verifyStatus.
     * @param appVerifyBaseInfo appVerifyBaseInfo.
     * @param skillUris skillUris.
     */
    void SingleVerify(const AppVerifyBaseInfo &appVerifyBaseInfo, const std::vector<SkillUri> &skillUris);

    /**
     * OnRemoteSaDied
     * @descrition
     * @param object systemAbility proxy object
     * @return void.
     */
    void OnRemoteSaDied(const wptr<IRemoteObject> &object);

private:
    bool IsServiceAvailable();
    void ConnectService();

private:
    class StaticDestoryMonitor {
    public:
        StaticDestoryMonitor()
            : destoryed_(false)
        {
        }
        ~StaticDestoryMonitor()
        {
            destoryed_ = true;
        }

        bool IsDestoryed() const
        {
            return destoryed_;
        }

    private:
        bool destoryed_;
    };

private:
    static std::mutex proxyLock_;
    static sptr<IAppDomainVerifyAgentService> agentServiceProxy_;
    static StaticDestoryMonitor staticDestoryMonitor_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;
};

class AgentSaDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit AgentSaDeathRecipient();
    virtual ~AgentSaDeathRecipient();
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;

private:
    DISALLOW_COPY_AND_MOVE(AgentSaDeathRecipient);
};
}
}
#endif