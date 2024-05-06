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

#include <string>
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
struct AppDomainVerifyModuleLabel {
    uint32_t domain;
    const char* tag;
};
static constexpr AppDomainVerifyModuleLabel APP_DOMAIN_VERIFY_MODULE_LABEL[APP_DOMAIN_VERIFY_MODULE_BUTT + 1] = {
    { APP_DOMAIN_VERIFY_EXTENSION_DOMAIN, "AppDomainVerifyExtension" },
    { APP_DOMAIN_VERIFY_MGR_CLIENT_DOMAIN, "AppDomainVerifyMgrClient" },
    { APP_DOMAIN_VERIFY_AGENT_CLIENT_DOMAIN, "AppDomainVerifyAgentClient" },
    { APP_DOMAIN_VERIFY_MGR_SERVICE_DOMAIN, "AppDomainVerifyMgrService" },
    { APP_DOMAIN_VERIFY_AGENT_SERVICE_DOMAIN, "AppDomainVerifyAgentService" },
    { APP_DOMAIN_VERIFY_JS_NAPI, "AppDomainVerifyJSNAPI" },
    { APP_DOMAIN_VERIFY_COMMON, "AppDomainVerifyCommon" },
    { APP_DOMAIN_VERIFY_BUTT, "AppDomainVerifytest" },
};

#ifndef APP_DOMAIN_VERIFY_FUNC_FMT
#define APP_DOMAIN_VERIFY_FUNC_FMT "[%{public}s(%{public}s:%{public}d)]"
#endif

#ifndef APP_DOMAIN_VERIFY_FILE_NAME
#define APP_DOMAIN_VERIFY_FILE_NAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifndef APP_DOMAIN_VERIFY_FUNC_INFO
#define APP_DOMAIN_VERIFY_FUNC_INFO APP_DOMAIN_VERIFY_FILE_NAME, __FUNCTION__, __LINE__
#endif

#define APP_DOMAIN_VERIFY_PRINT_LOG(level, label, fmt, ...)                                                     \
    ((void)HILOG_IMPL(LOG_CORE, level, APP_DOMAIN_VERIFY_MODULE_LABEL[label].domain,                            \
        APP_DOMAIN_VERIFY_MODULE_LABEL[label].tag, APP_DOMAIN_VERIFY_FUNC_FMT fmt, APP_DOMAIN_VERIFY_FUNC_INFO, \
        ##__VA_ARGS__))

#define APP_DOMAIN_VERIFY_HILOGD(label, fmt, ...) APP_DOMAIN_VERIFY_PRINT_LOG(LOG_DEBUG, label, fmt, ##__VA_ARGS__)
#define APP_DOMAIN_VERIFY_HILOGI(label, fmt, ...) APP_DOMAIN_VERIFY_PRINT_LOG(LOG_INFO, label, fmt, ##__VA_ARGS__)
#define APP_DOMAIN_VERIFY_HILOGW(label, fmt, ...) APP_DOMAIN_VERIFY_PRINT_LOG(LOG_WARN, label, fmt, ##__VA_ARGS__)
#define APP_DOMAIN_VERIFY_HILOGE(label, fmt, ...) APP_DOMAIN_VERIFY_PRINT_LOG(LOG_ERROR, label, fmt, ##__VA_ARGS__)
#define APP_DOMAIN_VERIFY_HILOGF(label, fmt, ...) APP_DOMAIN_VERIFY_PRINT_LOG(LOG_FATAL, label, fmt, ##__VA_ARGS__)
}  // namespace AppDomainVerify
} // namespace OHOS

#endif // APP_DOMAIN_VERIFY_HILOG_H
