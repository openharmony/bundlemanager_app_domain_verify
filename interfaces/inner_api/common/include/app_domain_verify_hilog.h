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
#ifndef APP_DOMAIN_VERIFY_HILOG_H
#define APP_DOMAIN_VERIFY_HILOG_H

#include "hilog/log.h"

namespace OHOS {
namespace AppDomainVerify {
// param of log interface.
enum AppDomainVerifySubModule {
    APP_DOMAIN_VERIFY_MODULE_EXTENSION = 0,
    APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT,
    APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT,
    APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE,
    APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE,
    APP_DOMAIN_VERIFY_MODULE_JS_NAPI,
    APP_DOMAIN_VERIFY_MODULE_COMMON,
    APP_DOMAIN_VERIFY_MODULE_BUTT,
};

// 0xD001C00: subsystem:APP_DOMAIN_VERIFY module:APP_DOMAIN_VERIFYManager, 8 bits reserved.
static constexpr unsigned int BASE_APP_DOMAIN_VERIFY_DOMAIN_ID = 0xD001C00;

enum AppDomainVerifyDomainId {
    APP_DOMAIN_VERIFY_EXTENSION_DOMAIN = BASE_APP_DOMAIN_VERIFY_DOMAIN_ID + APP_DOMAIN_VERIFY_MODULE_EXTENSION,
    APP_DOMAIN_VERIFY_MGR_CLIENT_DOMAIN,
    APP_DOMAIN_VERIFY_AGENT_CLIENT_DOMAIN,
    APP_DOMAIN_VERIFY_MGR_SERVICE_DOMAIN,
    APP_DOMAIN_VERIFY_AGENT_SERVICE_DOMAIN,
    APP_DOMAIN_VERIFY_JS_NAPI,
    APP_DOMAIN_VERIFY_COMMON,
    APP_DOMAIN_VERIFY_BUTT,
};

static constexpr OHOS::HiviewDFX::HiLogLabel APP_DOMAIN_VERIFY_MODULE_LABEL[APP_DOMAIN_VERIFY_MODULE_BUTT + 1] = {
    { LOG_CORE, APP_DOMAIN_VERIFY_EXTENSION_DOMAIN, "AppDomainVerifyExtension" },
    { LOG_CORE, APP_DOMAIN_VERIFY_MGR_CLIENT_DOMAIN, "AppDomainVerifyMgrClient" },
    { LOG_CORE, APP_DOMAIN_VERIFY_AGENT_CLIENT_DOMAIN, "AppDomainVerifyAgentClient" },
    { LOG_CORE, APP_DOMAIN_VERIFY_MGR_SERVICE_DOMAIN, "AppDomainVerifyMgrService" },
    { LOG_CORE, APP_DOMAIN_VERIFY_AGENT_SERVICE_DOMAIN, "AppDomainVerifyAgentService" },
    { LOG_CORE, APP_DOMAIN_VERIFY_JS_NAPI, "AppDomainVerifyJSNAPI" },
    { LOG_CORE, APP_DOMAIN_VERIFY_COMMON, "AppDomainVerifyCommon" },
    { LOG_CORE, APP_DOMAIN_VERIFY_BUTT, "AppDomainVerifytest" },
};

#define FILENAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define FORMATED(fmt, ...) "[%{public}s] %{public}s# " fmt, FILENAME, __FUNCTION__, ##__VA_ARGS__

// In order to improve performance, do not check the module range.
// Besides, make sure module is less than APP_DOMAIN_VERIFY_MODULE_BUTT.
#define APP_DOMAIN_VERIFY_HILOGF(module, ...) \
    (void)OHOS::HiviewDFX::HiLog::Fatal(APP_DOMAIN_VERIFY_MODULE_LABEL[module], FORMATED(__VA_ARGS__))
#define APP_DOMAIN_VERIFY_HILOGE(module, ...) \
    (void)OHOS::HiviewDFX::HiLog::Error(APP_DOMAIN_VERIFY_MODULE_LABEL[module], FORMATED(__VA_ARGS__))
#define APP_DOMAIN_VERIFY_HILOGW(module, ...) \
    (void)OHOS::HiviewDFX::HiLog::Warn(APP_DOMAIN_VERIFY_MODULE_LABEL[module], FORMATED(__VA_ARGS__))
#define APP_DOMAIN_VERIFY_HILOGI(module, ...) \
    (void)OHOS::HiviewDFX::HiLog::Info(APP_DOMAIN_VERIFY_MODULE_LABEL[module], FORMATED(__VA_ARGS__))
#define APP_DOMAIN_VERIFY_HILOGD(module, ...) \
    (void)OHOS::HiviewDFX::HiLog::Info(APP_DOMAIN_VERIFY_MODULE_LABEL[module], FORMATED(__VA_ARGS__))
} // namespace AppDomainVerify
} // namespace OHOS

#endif // APP_DOMAIN_VERIFY_HILOG_H
