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
#ifndef APP_DOMAIN_VERIFY_ERROR_H
#define APP_DOMAIN_VERIFY_ERROR_H

namespace OHOS {
namespace AppDomainVerify {
// todo 错误码与serviceid对应
enum ErrorCode : int32_t {
    E_OK,
    E_PERMISSION_DENIED = 201,
    E_IS_NOT_SYS_APP = 202,
    E_PARAM_ERROR = 401,
    E_EXTENSIONS_LIB_NOT_FOUND = 10000001,
    E_EXTENSIONS_INTERNAL_ERROR = 10000002,
    E_INTERNAL_ERR = 29900001,
    E_NET_CONNECT_ERR = 29900002,
    E_SERVER_ERR = 29900003,
};
}  // namespace AppDomainVerify
}  // namespace OHOS

#endif  // APP_DOMAIN_VERIFY_ERROR_H
