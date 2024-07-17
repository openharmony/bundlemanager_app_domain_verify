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

#include <memory>
#include "app_domain_verify_agent_service_stub.h"
#include "agent_interface_code.h"
#include "errors.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "parcel_util.h"

namespace OHOS {
namespace AppDomainVerify {
namespace {
constexpr int32_t DELAY_TIME = 300000;  // 5min = 5*60*1000
const std::string TASK_ID = "unload";
}
AppDomainVerifyAgentServiceStub::AppDomainVerifyAgentServiceStub()
{
}
AppDomainVerifyAgentServiceStub::~AppDomainVerifyAgentServiceStub()
{
}
int32_t AppDomainVerifyAgentServiceStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "onRemoteRequest##code = %{public}u", code);
    ExitIdleState();
    PostDelayUnloadTask();

    std::u16string myDescripter = AppDomainVerifyAgentServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "end##descriptor checked fail");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    switch (code) {
        case static_cast<uint32_t>(static_cast<uint32_t>(AgentInterfaceCode::SINGLE_VERIFY)):
            return OnSingleVerify(data, reply);
        case static_cast<uint32_t>(AgentInterfaceCode::CONVERT_TO_EXPLICIT_WANT):
            return OnConvertToExplicitWant(data, reply);
        default:
            APP_DOMAIN_VERIFY_HILOGW(
                APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "receive unknown code, code = %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int32_t AppDomainVerifyAgentServiceStub::OnSingleVerify(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    std::unique_ptr<AppVerifyBaseInfo> info(data.ReadParcelable<AppVerifyBaseInfo>());
    if (!info) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "read parcelable AppVerifyBaseInfo failed.");
        return ERR_INVALID_VALUE;
    }
    AppVerifyBaseInfo appVerifyBaseInfo = *info;
    int32_t skillUrisSize = data.ReadInt32();
    if (IsInvalidParcelArraySize(skillUrisSize)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable size invalid.");
        return false;
    }
    std::vector<SkillUri> skillUris;
    for (int32_t i = 0; i < skillUrisSize; i++) {
        std::unique_ptr<SkillUri> skillUri(data.ReadParcelable<SkillUri>());
        if (!skillUri) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "read parcelable skillUri failed.");
            return ERR_INVALID_VALUE;
        }
        skillUris.emplace_back(*skillUri);
    }
    SingleVerify(appVerifyBaseInfo, skillUris);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}
int32_t AppDomainVerifyAgentServiceStub::OnConvertToExplicitWant(MessageParcel& data, MessageParcel& reply)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    OHOS::AAFwk::Want want;
    std::unique_ptr<OHOS::AAFwk::Want> w(data.ReadParcelable<OHOS::AAFwk::Want>());
    if (!w) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read parcelable want failed.");
        return ERR_INVALID_VALUE;
    }
    want = *w;
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    if (object == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "read failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IConvertCallback> cleanCacheCallback = iface_cast<IConvertCallback>(object);
    ConvertToExplicitWant(want, cleanCacheCallback);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s call end", __func__);
    return ERR_OK;
}
void AppDomainVerifyAgentServiceStub::PostDelayUnloadTask()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    if (runner_ == nullptr) {
        runner_ = AppExecFwk::EventRunner::Create("unload", AppExecFwk::ThreadMode::FFRT);
    }
    if (unloadHandler_ == nullptr) {
        unloadHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    }
    if (unloadHandler_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "unloadHandler init failed!");
        return;
    }

    auto task = [this]() {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "do unload task");
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy == nullptr) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "get samgr failed");
            return;
        }
        int32_t ret = samgrProxy->UnloadSystemAbility(APP_DOMAIN_VERIFY_AGENT_SA_ID);
        if (ret != 0) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "remove system ability failed");
            return;
        }
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "do unload task done");
    };
    unloadHandler_->RemoveTask(TASK_ID);
    unloadHandler_->PostTask(task, TASK_ID, DELAY_TIME);
}

}  // namespace AppDomainVerify
}  // namespace OHOS