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
#ifndef APP_DOMAIN_VERIFY_MGR_INTERFACE_CODE_H
#define APP_DOMAIN_VERIFY_MGR_INTERFACE_CODE_H
namespace OHOS {
namespace AppDomainVerify {
enum AppDomainVerifyMgrInterfaceCode {
    QUERY_VERIFY_STATUS = 0,
    VERIFY_DOMAIN = 1,
    CLEAR_DOMAIN_VERIFY_RESULT = 2,
    FILTER_ABILITIES = 3,
    QUERY_ALL_VERIFY_STATUS = 4,
    SAVE_VERIFY_STATUS = 5,
    IS_ATOMIC_SERVICE_URL = 6,
    CONVERT_TO_EXPLICIT_WANT = 7,
    UPDATE_WHITE_LIST_URLS = 8,
    QUERY_ASSOCIATED_DOMAINS = 9,
    QUERY_ASSOCIATED_BUNDLE_NAMES = 10,
    GET_DEFERRED_LINK = 11,
    QUERY_APP_DETAILS_WANT = 12,
    IS_SHORT_URL = 13,
    CONVERT_FROM_SHORT_URL = 14,
};
}
}  // namespace OHOS

#endif