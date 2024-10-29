/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include <memory>
#include <map>
#include "app_domain_verify_manager_napi.h"
#include "app_domain_verify_hilog.h"
#include "app_domain_verify_mgr_client.h"
#include "comm_define.h"
#include "api_event_reporter.h"
#include "napi_value_utils.h"

namespace OHOS::AppDomainVerify {
using namespace Dfx;
std::map<CommonErrorCode, const char*> ErrCodeMap = { { CommonErrorCode::E_PERMISSION_DENIED, "Permission denied." },
    { CommonErrorCode::E_IS_NOT_SYS_APP, "System API accessed by non-system app." },
    { CommonErrorCode::E_PARAM_ERROR, "Parameter error." }, { CommonErrorCode::E_INTERNAL_ERR, "Internal error." } };

napi_value QueryAssociatedDomains(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = { nullptr };

    Dfx::ApiEventReporter reporter("QueryAssociatedDomains");
    NAPI_CALL_BASE(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr), nullptr);
    std::string bundleName = GetString(env, args[0]);
    if (!CheckInput(bundleName)) {
        reporter.WriteEndEvent(API_FAIL, CommonErrorCode::E_PARAM_ERROR);
        return BuildError(env, CommonErrorCode::E_PARAM_ERROR, ErrCodeMap[CommonErrorCode::E_PARAM_ERROR]);
    }
    std::vector<std::string> domains;
    auto ret = AppDomainVerifyMgrClient::GetInstance()->QueryAssociatedDomains(bundleName, domains);
    if (ret != 0) {
        if (ErrCodeMap.count(static_cast<CommonErrorCode>(ret)) != 0) {
            reporter.WriteEndEvent(API_FAIL, static_cast<CommonErrorCode>(ret));
            return BuildError(env, static_cast<CommonErrorCode>(ret), ErrCodeMap[static_cast<CommonErrorCode>(ret)]);
        } else {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "unknown error:%{public}d.", ret);
            reporter.WriteEndEvent(API_FAIL, ret);
            return BuildStringArray(env, domains);
        }
    }
    reporter.WriteEndEvent(API_SUCCESS, ret);
    return BuildStringArray(env, domains);
}
napi_value QueryAssociatedBundleNames(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = { nullptr };

    Dfx::ApiEventReporter reporter("QueryAssociatedBundleNames");
    NAPI_CALL_BASE(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr), nullptr);
    std::string domain = GetString(env, args[0]);
    if (!CheckInput(domain)) {
        reporter.WriteEndEvent(API_FAIL, CommonErrorCode::E_PARAM_ERROR);
        return BuildError(env, CommonErrorCode::E_PARAM_ERROR, ErrCodeMap[CommonErrorCode::E_PARAM_ERROR]);
    }
    std::vector<std::string> bundleNames;
    auto ret = AppDomainVerifyMgrClient::GetInstance()->QueryAssociatedBundleNames(domain, bundleNames);
    if (ret != 0) {
        if (ErrCodeMap.count(static_cast<CommonErrorCode>(ret)) != 0) {
            reporter.WriteEndEvent(API_FAIL, static_cast<CommonErrorCode>(ret));
            return BuildError(env, static_cast<CommonErrorCode>(ret), ErrCodeMap[static_cast<CommonErrorCode>(ret)]);
        } else {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "unknown error:%{public}d.", ret);
            reporter.WriteEndEvent(API_FAIL, ret);
            return BuildStringArray(env, bundleNames);
        }
    }
    reporter.WriteEndEvent(API_SUCCESS, ret);
    return BuildStringArray(env, bundleNames);
}
}