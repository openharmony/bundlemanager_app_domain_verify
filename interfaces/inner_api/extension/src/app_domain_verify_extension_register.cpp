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

#include "app_domain_verify_extension_register.h"
#include "app_domain_verify_hilog.h"

namespace OHOS {
namespace AppDomainVerify {
std::mutex AppDomainVerifyExtensionRegister::sMutex_;

AppDomainVerifyExtensionRegister::AppDomainVerifyExtensionRegister()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "new instance create.");
}

AppDomainVerifyExtensionRegister::~AppDomainVerifyExtensionRegister()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_EXTENSION, "instance dead.");
}

AppDomainVerifyExtensionRegister& AppDomainVerifyExtensionRegister::GetInstance()
{
    std::lock_guard<std::mutex> lock(sMutex_);
    static AppDomainVerifyExtensionRegister extRegister;
    return extRegister;
}
void AppDomainVerifyExtensionRegister::RegisterAppDomainVerifyExt(const std::string& extName,
    const CreateFunc& createFunc)
{
    std::lock_guard<std::mutex> lock(mutex_);
    verifyExts_.emplace(extName, createFunc);
}
std::shared_ptr<AppDomainVerifyAgentExt> AppDomainVerifyExtensionRegister::GetAppDomainVerifyExt(
    const std::string& extName)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = verifyExts_.find(extName);
    if (it == verifyExts_.end()) {
        return nullptr;
    }
    return it->second();
}
}
}
