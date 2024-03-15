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
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "app_domain_verify_agent_client.h"

namespace OHOS {
namespace AppDomainVerify {
constexpr int32_t LOADSA_TIMEOUT_MS = 10000;
sptr<IAppDomainVerifyAgentService> AppDomainVerifyAgentClient::agentServiceProxy_;
AppDomainVerifyAgentClient::StaticDestoryMonitor AppDomainVerifyAgentClient::staticDestoryMonitor_;
std::mutex AppDomainVerifyAgentClient::proxyLock_;

AppDomainVerifyAgentClient::AppDomainVerifyAgentClient()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "new instance created.");
}

AppDomainVerifyAgentClient::~AppDomainVerifyAgentClient()
{
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (agentServiceProxy_ != nullptr && !staticDestoryMonitor_.IsDestoryed()) {
        auto remoteObject = agentServiceProxy_->AsObject();
        if (remoteObject != nullptr) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
        }
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "instance dead.");
}

void AppDomainVerifyAgentClient::SingleVerify(const AppVerifyBaseInfo &appVerifyBaseInfo,
    const std::vector<SkillUri> &skillUris)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "%s called", __func__);
    if (IsServiceAvailable()) {
        agentServiceProxy_->SingleVerify(appVerifyBaseInfo, skillUris);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "%s call end", __func__);
}

bool AppDomainVerifyAgentClient::IsServiceAvailable()
{
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (agentServiceProxy_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "Redo ConnectService");
        ConnectService();
    }
    if (agentServiceProxy_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "Service proxy null.");
        return false;
    }
    return true;
}

void AppDomainVerifyAgentClient::ConnectService()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "ConnectService start.");
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "Get SystemAbilityManager failed.");
        agentServiceProxy_ = nullptr;
        return;
    }
    sptr<IRemoteObject> remoteObject = samgrProxy->CheckSystemAbility(APP_DOMAIN_VERIFY_AGENT_SA_ID);
    if (remoteObject == nullptr) {
        remoteObject = samgrProxy->LoadSystemAbility(APP_DOMAIN_VERIFY_AGENT_SA_ID, LOADSA_TIMEOUT_MS);
    }
    if (remoteObject != nullptr) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "Get AgentServiceProxy succeed.");
        if (deathRecipient_ == nullptr) {
            deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new AgentSaDeathRecipient());
        }
        remoteObject->AddDeathRecipient(deathRecipient_);
        agentServiceProxy_ = iface_cast<IAppDomainVerifyAgentService>(remoteObject);
        return;
    }
    APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "Getting AgentServiceProxy failed.");
}

void AppDomainVerifyAgentClient::OnRemoteSaDied(const wptr<IRemoteObject> &object)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "OnRemoteSaDied.");
    std::lock_guard<std::mutex> autoLock(proxyLock_);
    if (agentServiceProxy_ != nullptr) {
        auto remoteObj = agentServiceProxy_->AsObject();
        if (remoteObj != nullptr) {
            remoteObj->RemoveDeathRecipient(deathRecipient_);
        }
    }
    agentServiceProxy_ = nullptr;
}

AgentSaDeathRecipient::AgentSaDeathRecipient()
{
}

AgentSaDeathRecipient::~AgentSaDeathRecipient()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT, "deathRecipient dead.");
}

void AgentSaDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT,
        "AppDomainVerifyMgrSaDeathRecipient on remote systemAbility died.");
    AppDomainVerifyAgentClient::GetInstance()->OnRemoteSaDied(object);
}
}
}