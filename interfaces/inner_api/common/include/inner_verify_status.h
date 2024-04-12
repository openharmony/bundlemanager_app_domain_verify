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

#ifndef APP_DOMAIN_INNER_VERIFY_STATUS_H
#define APP_DOMAIN_INNER_VERIFY_STATUS_H
namespace OHOS {
namespace AppDomainVerify {
enum InnerVerifyStatus {
    /**
     * Default value.
     */
    UNKNOWN = 0,

    STATE_SUCCESS = 1,

    STATE_FAIL = 2,

    FAILURE_REDIRECT = 3,

    FAILURE_CLIENT_ERROR = 4,

    FAILURE_REJECTED_BY_SERVER = 5,

    FAILURE_HTTP_UNKNOWN = 6,

    FAILURE_TIMEOUT = 7,
};
static std::unordered_map<InnerVerifyStatus, std::string> InnerVerifyStatusMap = {
    { UNKNOWN, "unknown" },
    { STATE_SUCCESS, "success" },
    { STATE_FAIL, "fail" },
    { FAILURE_REDIRECT, "redirect" },
    { FAILURE_CLIENT_ERROR, "client_error" },
    { FAILURE_REJECTED_BY_SERVER, "redirect_by_server" },
    { FAILURE_HTTP_UNKNOWN, "http_unknown" },
    { FAILURE_TIMEOUT, "timeout" },
};
}
}  // namespace OHOS

#endif  // APP_DOMAIN_VERIFY_STATUS_H