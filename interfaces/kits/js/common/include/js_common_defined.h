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

#ifndef APP_DOMAIN_VERIFY_JS_COMMON_DEFINED_H
#define APP_DOMAIN_VERIFY_JS_COMMON_DEFINED_H
#include <map>
#include "app_domain_verify_error.h"

namespace OHOS::AppDomainVerify {
constexpr int32_t MAX_STR_INPUT_SIZE = 256;
constexpr int32_t STRING_BUF_MAX_SIZE = 4096;
static std::map<ErrorCode, const char*> ErrCodeMap = { { ErrorCode::E_PERMISSION_DENIED, "Permission denied." },
    { ErrorCode::E_IS_NOT_SYS_APP, "System API accessed by non-system app." },
    { ErrorCode::E_PARAM_ERROR, "Parameter error." }, { ErrorCode::E_INTERNAL_ERR, "Internal error." } };
}
#endif  // APP_DOMAIN_VERIFY_JS_COMMON_DEFINED_H
