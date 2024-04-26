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

#ifndef APP_DOMAIN_VERIFY_EXTENSION_REGISTER_H
#define APP_DOMAIN_VERIFY_EXTENSION_REGISTER_H
#include <mutex>
#include <memory>
#include <functional>
#include "app_domain_verify_agent_ext.h"
#include "nocopyable.h"

namespace OHOS {
namespace AppDomainVerify {
#define REGISTER_VERIFIER_EXT(baseClassName, className)                                                  \
    __attribute__((constructor)) void RegisterVerifierExt##className()                                   \
    {                                                                                                    \
        AppDomainVerifyExtensionRegister::GetInstance().RegisterAppDomainVerifyExt(#baseClassName,       \
            []() -> std::shared_ptr<AppDomainVerifyAgentExt> { return std::make_shared<className>(); }); \
    }

using CreateFunc = std::function<std::shared_ptr<AppDomainVerifyAgentExt>(void)>;
class AppDomainVerifyExtensionRegister {
public:
    static AppDomainVerifyExtensionRegister& GetInstance();
    ~AppDomainVerifyExtensionRegister();
    void RegisterAppDomainVerifyExt(const std::string& extName, const CreateFunc& createFunc);
    std::shared_ptr<AppDomainVerifyAgentExt> GetAppDomainVerifyExt(const std::string& extName);

private:
    static std::mutex sMutex_;
    mutable std::mutex mutex_;
    AppDomainVerifyExtensionRegister();
    std::unordered_map<std::string, CreateFunc> verifyExts_;

    DISALLOW_COPY_AND_MOVE(AppDomainVerifyExtensionRegister);
};
}
}
#endif