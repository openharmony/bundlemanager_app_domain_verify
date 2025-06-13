/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "critial_utils.h"
#include "mem_mgr_client.h"
#include "mem_mgr_proxy.h"
#include "system_ability_definition.h"
#include "app_domain_verify_hilog.h"
namespace OHOS::AppDomainVerify {
using namespace OHOS::Memory;
constexpr int TYPE_SA = 1;
CriticalUtils& CriticalUtils::GetInstance()
{
    static CriticalUtils instance;
    return instance;
}
void CriticalUtils::AddCritical()
{
    std::unique_lock<std::mutex> lock;
    if (count_ == 0) {
        SetCritical(true);
    }
    ++count_;
}
void CriticalUtils::RemoveCritical()
{
    std::unique_lock<std::mutex> lock;
    --count_;
    if (count_ == 0) {
        SetCritical(false);
    }
}
void CriticalUtils::NotifyProcessStatus(const int status) const
{
    auto ret = MemMgrClient::GetInstance().NotifyProcessStatus(pid_, TYPE_SA, status, APP_DOMAIN_VERIFY_AGENT_SA_ID);
    if (ret == 0) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "NotifyProcessStatus status:%{public}d", status);
    } else {
        APP_DOMAIN_VERIFY_HILOGW(
            APP_DOMAIN_VERIFY_MODULE_COMMON, "NotifyProcessStatus status:%{public}d failed", status);
    }
}
void CriticalUtils::SetCritical(bool isCritical) const
{
    auto ret = MemMgrClient::GetInstance().SetCritical(pid_, isCritical, APP_DOMAIN_VERIFY_AGENT_SA_ID);
    if (ret == 0) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "SetCritical isCritical:%{public}d", isCritical);
    } else {
        APP_DOMAIN_VERIFY_HILOGW(
            APP_DOMAIN_VERIFY_MODULE_COMMON, "SetCritical isCritical:%{public}d failed", isCritical);
    }
}
CriticalUtils::CriticalUtils()
{
    pid_ = getpid();
}
CriticalLock::CriticalLock()
{
    CriticalUtils::GetInstance().AddCritical();
}
CriticalLock::~CriticalLock()
{
    CriticalUtils::GetInstance().RemoveCritical();
}
}