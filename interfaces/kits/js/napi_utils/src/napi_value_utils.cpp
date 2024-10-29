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

#include "napi_value_utils.h"
namespace OHOS::AppDomainVerify {
constexpr int32_t MAX_STR_INPUT_SIZE = 256;
constexpr int32_t STRING_BUF_MAX_SIZE = 4096;
std::string GetString(napi_env env, napi_value value)
{
    std::unique_ptr<char[]> valueBuf = std::make_unique<char[]>(STRING_BUF_MAX_SIZE);
    size_t size = 0;
    NAPI_CALL_BASE(env, napi_get_value_string_utf8(env, value, valueBuf.get(), STRING_BUF_MAX_SIZE, &size), "");
    std::string result = std::string(valueBuf.get(), size);
    return result;
}
napi_value BuildString(const napi_env& env, const std::string& data)
{
    napi_value result;
    NAPI_CALL_BASE(env, napi_create_string_utf8(env, data.c_str(), NAPI_AUTO_LENGTH, &result), nullptr);
    return result;
}
napi_value BuildStringArray(const napi_env& env, const std::vector<std::string>& data)
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
bool CheckInput(const std::string& input)
{
    if (input.empty() || input.size() > MAX_STR_INPUT_SIZE) {
        return false;
    }
    return true;
}
napi_value BuildError(const napi_env& env, uint32_t errorCode, const char* errorMsg)
{
    auto ret = napi_throw_error(env, std::to_string(errorCode).c_str(), errorMsg);
    if (ret != napi_status::napi_ok) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "throw err failed.");
    }
    return nullptr;
}
}