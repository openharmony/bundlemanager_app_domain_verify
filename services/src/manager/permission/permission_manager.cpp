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
#include "permission_manager.h"
#include "app_domain_verify_hilog.h"
#include "ipc_skeleton.h"
#include "accesstoken_kit.h"
#include "tokenid_kit.h"
namespace OHOS::AppDomainVerify {

bool PermissionManager::CheckPermission(const std::string& permission)
{
    if (permission.empty()) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "permission empty.");
        return false;
    }
    auto callerToken = IPCSkeleton::GetCallingTokenID();
    int result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permission);
    if (result != Security::AccessToken::PERMISSION_GRANTED) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE,
            "permission check failed, permission:%{public}s, callerToken:%{public}u", permission.c_str(), callerToken);
        return false;
    }
    return true;
}
bool PermissionManager::IsSystemAppCall()
{
    if (Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(IPCSkeleton::GetCallingTokenID()) !=
        Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        return true;
    }
    auto result = Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(IPCSkeleton::GetCallingFullTokenID());
    if (!result) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "Caller is not allowed, need system app");
    }
    return result;
}
}