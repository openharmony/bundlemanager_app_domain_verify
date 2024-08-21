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
namespace OHOS::AppDomainVerify {
constexpr int32_t MAX_STR_INPUT_SIZE = 256;
constexpr int32_t STRING_BUF_MAX_SIZE = 4096;
std::map<CommonErrorCode, const char*> ErrCodeMap = { { CommonErrorCode::E_PERMISSION_DENIED, "Permission denied." },
    { CommonErrorCode::E_IS_NOT_SYS_APP, "System API accessed by non-system app." },
    { CommonErrorCode::E_PARAM_ERROR, "Parameter error." }, { CommonErrorCode::E_INTERNAL_ERR, "Internal error." } };
static std::string GetString(napi_env env, napi_value value)
{
    std::unique_ptr<char[]> valueBuf = std::make_unique<char[]>(STRING_BUF_MAX_SIZE);
    size_t size = 0;
    NAPI_CALL_BASE(env, napi_get_value_string_utf8(env, value, valueBuf.get(), STRING_BUF_MAX_SIZE, &size), "");
    std::string result = std::string(valueBuf.get(), size);
    return result;
}
static napi_value BuildString(const napi_env& env, const std::string& data)
{
    napi_value result;
    NAPI_CALL_BASE(env, napi_create_string_utf8(env, data.c_str(), NAPI_AUTO_LENGTH, &result), nullptr);
    return result;
}
static napi_value BuildStringArray(const napi_env& env, const std::vector<std::string>& data)
{
    napi_value arr;
    NAPI_CALL_BASE(env, napi_create_array(env, &arr), nullptr);
    size_t index = 0;
    for (auto&& str : data) {
        napi_value value = BuildString(env, str);
        NAPI_CALL_BASE(env, napi_set_element(env, arr, index++, value), nullptr);
    }
    return arr;
}
static bool CheckInput(const std::string& input)
{
    if (input.empty() || input.size() > MAX_STR_INPUT_SIZE) {
        return false;
    }
    return true;
}
static napi_value BuildError(const napi_env& env, CommonErrorCode errorCode)
{
    auto ret = napi_throw_error(env, std::to_string(errorCode).c_str(), ErrCodeMap[errorCode]);
    if (ret != napi_status::napi_ok) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "throw err failed.");
    }
    return nullptr;
}
napi_value QueryAssociatedDomains(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = { nullptr };

    Dfx::ApiEventReporter reporter("QueryAssociatedDomains");
    NAPI_CALL_BASE(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr), nullptr);
    std::string bundleName = GetString(env, args[0]);
    if (!CheckInput(bundleName)) {
        return BuildError(env, CommonErrorCode::E_PARAM_ERROR);
    }
    std::vector<std::string> domains;
    auto ret = AppDomainVerifyMgrClient::GetInstance()->QueryAssociatedDomains(bundleName, domains);
    if (ret != 0) {
        if (ErrCodeMap.count(static_cast<CommonErrorCode>(ret)) != 0) {
            return BuildError(env, static_cast<CommonErrorCode>(ret));
        } else {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "unknown error:%{public}d.", ret);
            return BuildStringArray(env, domains);
        }
    }
    reporter.WriteEvent(0, 0);
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
        return BuildError(env, CommonErrorCode::E_PARAM_ERROR);
    }
    std::vector<std::string> bundleNames;
    auto ret = AppDomainVerifyMgrClient::GetInstance()->QueryAssociatedBundleNames(domain, bundleNames);
    if (ret != 0) {
        if (ErrCodeMap.count(static_cast<CommonErrorCode>(ret)) != 0) {
            return BuildError(env, static_cast<CommonErrorCode>(ret));
        } else {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "unknown error:%{public}d.", ret);
            return BuildStringArray(env, bundleNames);
        }
    }
    reporter.WriteEvent(0, 0);
    return BuildStringArray(env, bundleNames);
}
}