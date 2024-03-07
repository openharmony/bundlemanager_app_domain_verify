/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef APP_DOMAIN_VERIFY_MGR_CONSTANTS_H
#define APP_DOMAIN_VERIFY_MGR_CONSTANTS_H

#include <string>

namespace OHOS {
namespace AppDomainVerify {
namespace Constants {
constexpr const char *SERVICE_PATH = "/data/service/el1/public/app_domain_verify_mgr_service";
const int32_t RDB_VERSION = 1;

constexpr const char *RDB_NAME = "/advdb.db";
constexpr const char *RDB_TABLE_NAME = "verified_domain";
}  // namespace Constants
}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_CONSTANTS_H