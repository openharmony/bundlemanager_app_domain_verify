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
enum AppDomainVerifySubModule
{
    APP_DOMAIN_VERIFY_MODULE_EXTENSION = 0,
    APP_DOMAIN_VERIFY_MGR_MODULE_CLIENT,
    APP_DOMAIN_VERIFY_AGENT_MODULE_CLIENT,
    APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE,
    APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE,
    APP_DOMAIN_VERIFY_MODULE_JS_NAPI,
    APP_DOMAIN_VERIFY_MODULE_COMMON,
    APP_DOMAIN_VERIFY_MODULE_BUTT,
};

static constexpr unsigned int APP_DOMAIN_VERIFY_DOMAIN_ID = 0xD0011FD;

static constexpr const char* APP_DOMAIN_VERIFY_MODULE_LABEL[APP_DOMAIN_VERIFY_MODULE_BUTT + 1] = {
    "ADVExt",
    "ADVMgrClient",
    "ADVAgtClient",
    "ADVMgrSrv",
    "ADVAgtSrv",
    "ADVNapi",
    "ADVComm",
    "ADVButt",
};
#ifdef IS_RELEASE_VERSION
#ifndef APP_DOMAIN_VERIFY_FUNC_FMT
#define APP_DOMAIN_VERIFY_FUNC_FMT "[(%{public}s:%{public}d)]"
#endif

#ifndef APP_DOMAIN_VERIFY_FUNC_INFO
#define APP_DOMAIN_VERIFY_FUNC_INFO __FUNCTION__, __LINE__
#endif
#else  // IS_RELEASE_VERSION
#ifndef APP_DOMAIN_VERIFY_FUNC_FMT
#define APP_DOMAIN_VERIFY_FUNC_FMT "[%{public}s(%{public}s:%{public}d)]"
#endif

#ifndef APP_DOMAIN_VERIFY_FILE_NAME
#define APP_DOMAIN_VERIFY_FILE_NAME \
    (__builtin_strrchr(__FILE_NAME__, '/') ? __builtin_strrchr(__FILE_NAME__, '/') + 1 : __FILE_NAME__)
#endif

#ifndef APP_DOMAIN_VERIFY_FUNC_INFO
#define APP_DOMAIN_VERIFY_FUNC_INFO APP_DOMAIN_VERIFY_FILE_NAME, __FUNCTION__, __LINE__
#endif
#endif  // IS_RELEASE_VERSION

#define APP_DOMAIN_VERIFY_PRINT_LOG(level, label, fmt, ...)                                                \
    ((void)HILOG_IMPL(LOG_CORE, level, APP_DOMAIN_VERIFY_DOMAIN_ID, APP_DOMAIN_VERIFY_MODULE_LABEL[label], \
        APP_DOMAIN_VERIFY_FUNC_FMT fmt, APP_DOMAIN_VERIFY_FUNC_INFO, ##__VA_ARGS__))

#define APP_DOMAIN_VERIFY_HILOGD(label, fmt, ...) APP_DOMAIN_VERIFY_PRINT_LOG(LOG_DEBUG, label, fmt, ##__VA_ARGS__)
#define APP_DOMAIN_VERIFY_HILOGI(label, fmt, ...) APP_DOMAIN_VERIFY_PRINT_LOG(LOG_INFO, label, fmt, ##__VA_ARGS__)
#define APP_DOMAIN_VERIFY_HILOGW(label, fmt, ...) APP_DOMAIN_VERIFY_PRINT_LOG(LOG_WARN, label, fmt, ##__VA_ARGS__)
#define APP_DOMAIN_VERIFY_HILOGE(label, fmt, ...) APP_DOMAIN_VERIFY_PRINT_LOG(LOG_ERROR, label, fmt, ##__VA_ARGS__)
#define APP_DOMAIN_VERIFY_HILOGF(label, fmt, ...) APP_DOMAIN_VERIFY_PRINT_LOG(LOG_FATAL, label, fmt, ##__VA_ARGS__)

}  // namespace AppDomainVerify
}  // namespace OHOS

#endif  // APP_DOMAIN_VERIFY_HILOG_H
