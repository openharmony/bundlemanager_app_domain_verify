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
#ifndef APP_VERIFY_PARCEL_UTIL_H
#define APP_VERIFY_PARCEL_UTIL_H

#include "app_domain_verify_hilog.h"
#include "comm_define.h"
#include "string_ex.h"

#define WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(type, parcel, data)                                              \
    do {                                                                                                       \
        if (!(parcel).Write##type(data)) {                                                                     \
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "fail to write %{public}s type", #type); \
            return false;                                                                                      \
        }                                                                                                      \
    } while (0)

#define WRITE_PARCEL_AND_RETURN_IF_FAIL(type, parcel, data)                                                    \
    do {                                                                                                       \
        if (!(parcel).Write##type(data)) {                                                                     \
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "fail to write %{public}s type", #type); \
            return;                                                                                            \
        }                                                                                                      \
    } while (0)

#define WRITE_PARCEL_AND_RETURN_INT_IF_FAIL(type, parcel, data)                                                \
    do {                                                                                                       \
        if (!(parcel).Write##type(data)) {                                                                     \
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "fail to write %{public}s type", #type); \
            return CommonErrorCode::E_INTERNAL_ERR;                                                            \
        }                                                                                                      \
    } while (0)

#define READ_PARCEL_AND_RETURN_INT_IF_FAIL(type, parcel, data)                                                \
    do {                                                                                                      \
        if (!(parcel).Read##type(data)) {                                                                     \
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "fail to read %{public}s type", #type); \
            return CommonErrorCode::E_INTERNAL_ERR;                                                           \
        }                                                                                                     \
    } while (0)

#define READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(type, parcel, data)                                              \
    do {                                                                                                      \
        if (!(parcel).Read##type(data)) {                                                                     \
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "fail to read %{public}s type", #type); \
            return false;                                                                                     \
        }                                                                                                     \
    } while (0)

#define MAX_PARCEL_ARRAY_SIZE (99)
namespace OHOS::AppDomainVerify {
inline bool IsInvalidParcelArraySize(int size)
{
    if (size > MAX_PARCEL_ARRAY_SIZE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "is invalid parcel array size %{public}d", size);
        return true;
    }
    return false;
}
}
#endif