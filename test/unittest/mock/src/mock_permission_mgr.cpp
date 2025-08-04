/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "mock_permission_mgr.h"
#include "permission_manager.h"
#include <utility>
namespace OHOS::AppDomainVerify {
static std::shared_ptr<MocIPermissionManager> g_mocPermissionManager = nullptr;
void DoMocPermissionManager(std::shared_ptr<MocIPermissionManager> moc)
{
    g_mocPermissionManager = std::move(moc);
}
bool PermissionManager::CheckPermission(const std::string& permission)
{
    if (g_mocPermissionManager) {
        return g_mocPermissionManager->CheckPermission(permission);
    }
    return true;
}
bool PermissionManager::IsSystemAppCall()
{
    if (g_mocPermissionManager) {
        return g_mocPermissionManager->IsSystemAppCall();
    }
    return true;
}
bool PermissionManager::IsSACall()
{
    if (g_mocPermissionManager) {
        return g_mocPermissionManager->IsSACall();
    }
    return true;
}
bool PermissionManager::IsAgentCall()
{
    if (g_mocPermissionManager) {
        return g_mocPermissionManager->IsAgentCall();
    }
    return true;
}
}