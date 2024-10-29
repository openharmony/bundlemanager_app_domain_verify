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

#ifndef APP_DOMAIN_VERIFY_NAPI_ASYNC_UTILS_H
#define APP_DOMAIN_VERIFY_NAPI_ASYNC_UTILS_H
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "app_domain_verify_hilog.h"
namespace OHOS::AppDomainVerify {
constexpr int SUCCESS = 0;
constexpr int ARGS_SIZE_ZERO = 0;
constexpr int ARGS_SIZE_ONE = 1;
constexpr int ARGS_SIZE_TWO = 2;
struct AsyncWorkData {
    explicit AsyncWorkData(napi_env napiEnv) : env(napiEnv){};
    virtual ~AsyncWorkData();
    napi_env env;
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;
};

struct BaseCallbackInfo : public AsyncWorkData {
    explicit BaseCallbackInfo(napi_env napiEnv) : AsyncWorkData(napiEnv)
    {
    }
    int32_t err = 0;
    std::string message;
};

template <typename T>
static napi_value AsyncCallNativeMethod(napi_env env, T* asyncCallbackInfo, const std::string& methodName,
    void (*execFunc)(napi_env, void*), void (*completeFunc)(napi_env, napi_status, void*))
{
    if (asyncCallbackInfo == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "asyncCallbackInfo is null");
        return nullptr;
    }
    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, methodName.c_str(), NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env,
        napi_create_async_work(env, nullptr, resource, execFunc, completeFunc,
            reinterpret_cast<void*>(asyncCallbackInfo), &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    return promise;
}
}
#endif  // APP_DOMAIN_VERIFY_NAPI_ASYNC_UTILS_H
