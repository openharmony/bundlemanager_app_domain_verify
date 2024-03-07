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
#include "app_domain_verify_agent_service_proxy.h"
#include "agent_interface_code.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppDomainVerify {
AppDomainVerifyAgentServiceProxy::AppDomainVerifyAgentServiceProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IAppDomainVerifyAgentService>(object)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "new instance created.");
}
AppDomainVerifyAgentServiceProxy::~AppDomainVerifyAgentServiceProxy()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "instance dead.");
}

void AppDomainVerifyAgentServiceProxy::CompleteVerifyRefresh(const BundleVerifyStatusInfo &bundleVerifyStatusInfo,
    const std::vector<InnerVerifyStatus> &statuses, int delaySeconds)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "failed to write interfaceToken");
        return;
    }

    if (!data.WriteParcelable(&bundleVerifyStatusInfo)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "failed to write parcelable");
        return;
    }

    uint32_t size = static_cast<uint32_t>(statuses.size());
    if (!data.WriteUint32(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "failed to write uint32_t");
        return;
    }

    for (uint32_t i = 0; i < statuses.size(); ++i) {
        if (!data.WriteInt32(statuses[i])) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "failed to write int32");
            return;
        }
    }
    if (!data.WriteInt32(delaySeconds)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "failed to write int32");
        return;
    }

    int32_t error = Remote()->SendRequest(AgentInterfaceCode::COMPLETE_VERIFY_REFRESH, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "CompleteVerifyRefresh failed, error: %d",
            error);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}

void AppDomainVerifyAgentServiceProxy::SingleVerify(const AppVerifyBaseInfo &appVerifyBaseInfo,
    const std::vector<SkillUri> &skillUris)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "failed to write interfaceToken");
        return;
    }

    if (!data.WriteParcelable(&appVerifyBaseInfo)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "failed to write parcelable");
        return;
    }

    uint32_t size = static_cast<uint32_t>(skillUris.size());
    if (!data.WriteUint32(size)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "failed to write uint32_t");
        return;
    }

    for (uint32_t i = 0; i < skillUris.size(); ++i) {
        if (!data.WriteParcelable(&skillUris[i])) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "failed to write raw data");
            return;
        }
    }
    int32_t error = Remote()->SendRequest(AgentInterfaceCode::SINGLE_VERIFY, data, reply, option);
    if (error != ERR_NONE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "SingleVerify failed, error: %d", error);
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
}
}  // namespace AppDomainVerify
}  // namespace OHOS
