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

#include <dlfcn.h>
#include <string>
#include <typeinfo>
#include "app_domain_verify_extension_mgr.h"
#include "app_domain_verify_hilog.h"
#include "app_domain_verify_extension_register.h"
#include "app_domain_verify_agent_ext.h"

namespace OHOS {
namespace AppDomainVerify {

#ifdef APP_USE_ARM64
const std::string EXTENSION_LIB_PATH = "/system/lib64/libapp_domain_verify_extension.z.so";
#elif defined(APP_USE_X86_64)
const std::string EXTENSION_LIB_PATH = "/system/lib64/libapp_domain_verify_extension.z.so";
#else
const std::string EXTENSION_LIB_PATH = "/system/lib/libapp_domain_verify_extension.z.so";
#endif

std::mutex AppDomainVerifyExtensionMgr::sHandlerMutex;
void* AppDomainVerifyExtensionMgr::sHandler = nullptr;

AppDomainVerifyExtensionMgr::AppDomainVerifyExtensionMgr()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "new instance create.");
}
AppDomainVerifyExtensionMgr::~AppDomainVerifyExtensionMgr()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "instance dead.");
}
bool AppDomainVerifyExtensionMgr::Init()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "%{public}s called.", __func__);
    std::lock_guard<std::mutex> lock(sHandlerMutex);
    auto handle = &sHandler;
    if (*handle == nullptr) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "dlopen extension lib");
        *handle = OpenLib();
        if (*handle == nullptr) {
            APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "failed to open %{public}s, err:%{public}s",
                EXTENSION_LIB_PATH.c_str(), dlerror());
            return false;
        }
    }
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "%{public}s call end.", __func__);
    return true;
}

ErrorCode AppDomainVerifyExtensionMgr::CompleteVerifyRefresh(const BundleVerifyStatusInfo& bundleVerifyStatusInfo,
    const std::vector<InnerVerifyStatus>& statuses, int delaySeconds, TaskType type)
{
    if (Init()) {
        std::string verifierExtName = APP_DOMAIN_VERIFY_AGENT_EXT_NAME;
        auto appDomainVerifierExt = GetAppDomainVerifyExt(verifierExtName);
        if (appDomainVerifierExt == nullptr) {
            APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "get verifierExt: %{public}s failed.",
                verifierExtName.c_str());
            return ErrorCode::E_EXTENSIONS_INTERNAL_ERROR;
        }
        return std::static_pointer_cast<AppDomainVerifyAgentExt>(appDomainVerifierExt)
            ->CompleteVerifyRefresh(bundleVerifyStatusInfo, statuses, delaySeconds, type);
    }
    return ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND;
}

ErrorCode AppDomainVerifyExtensionMgr::SingleVerify(const AppVerifyBaseInfo& appVerifyBaseInfo,
    const std::vector<SkillUri>& skillUris)
{
    if (Init()) {
        std::string verifierExtName = APP_DOMAIN_VERIFY_AGENT_EXT_NAME;
        auto appDomainVerifierExt = GetAppDomainVerifyExt(verifierExtName);
        if (appDomainVerifierExt != nullptr) {
            return std::static_pointer_cast<AppDomainVerifyAgentExt>(appDomainVerifierExt)
                ->SingleVerify(appVerifyBaseInfo, skillUris);
        }
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "get verifierExt: %{public}s failed.",
            verifierExtName.c_str());
        return ErrorCode::E_EXTENSIONS_INTERNAL_ERROR;
    }
    return ErrorCode::E_EXTENSIONS_LIB_NOT_FOUND;
}
std::shared_ptr<AppDomainVerifyAgentExt> AppDomainVerifyExtensionMgr::GetAppDomainVerifyExt(const std::string& extName)
{
    return AppDomainVerifyExtensionRegister::GetInstance().GetAppDomainVerifyExt(extName);
}
void* AppDomainVerifyExtensionMgr::OpenLib()
{
    return dlopen(EXTENSION_LIB_PATH.c_str(), RTLD_NOW | RTLD_GLOBAL);
}
}
}