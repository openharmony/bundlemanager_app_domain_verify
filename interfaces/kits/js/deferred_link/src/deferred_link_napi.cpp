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
#include "deferred_link_napi.h"
#include "app_domain_verify_hilog.h"
#include "app_domain_verify_mgr_client.h"
#include "comm_define.h"
#include "api_event_reporter.h"
#include "napi_value_utils.h"
#include "napi_async_utils.h"

namespace OHOS::AppDomainVerify {
using namespace Dfx;
namespace {
constexpr const char* GET_DEFERRED_LINK = "GetDeferredLink";
}
std::map<CommonErrorCode, const char*> ErrCodeMap = { { CommonErrorCode::E_INTERNAL_ERR, "Internal error." } };

struct GetDeferredLinkCallbackInfo : public BaseCallbackInfo {
    GetDeferredLinkCallbackInfo(napi_env napiEnv, std::unique_ptr<Dfx::ApiEventReporter> reporter)
        : BaseCallbackInfo(napiEnv), apiReporter(std::move(reporter))
    {
    }
    std::unique_ptr<Dfx::ApiEventReporter> apiReporter;
    std::string deferred_link;
};

void GetDeferredLinkExec(napi_env env, void* data)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    auto* asyncCallbackInfo = reinterpret_cast<GetDeferredLinkCallbackInfo*>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "asyncCallbackInfo is null");
        return;
    }
    std::string deferredLink;
    asyncCallbackInfo->err = AppDomainVerifyMgrClient::GetInstance()->GetDeferredLink(deferredLink);
    asyncCallbackInfo->deferred_link = deferredLink;
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called end");
}

void GetDeferredLinkComplete(napi_env env, napi_status status, void* data)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    auto* asyncCallbackInfo = reinterpret_cast<GetDeferredLinkCallbackInfo*>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "asyncCallbackInfo is null in %{public}s", __func__);
        return;
    }
    std::unique_ptr<GetDeferredLinkCallbackInfo> callbackPtr{ asyncCallbackInfo };
    napi_value result[ARGS_SIZE_TWO] = { nullptr };
    if (asyncCallbackInfo->err == SUCCESS) {
        NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &result[0]));
        result[ARGS_SIZE_ONE] = BuildString(env, asyncCallbackInfo->deferred_link);
    } else {
        result[0] = BuildError(env, CommonErrorCode::E_INTERNAL_ERR, "internal error.");
    }
    if (asyncCallbackInfo->deferred) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "is deferred");
        if (asyncCallbackInfo->err == SUCCESS) {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[ARGS_SIZE_ONE]));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
        }
    } else {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "is callback");
        napi_value callback = nullptr;
        napi_value placeHolder = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
        NAPI_CALL_RETURN_VOID(
            env, napi_call_function(env, nullptr, callback, sizeof(result) / sizeof(result[0]), result, &placeHolder));
    }
    callbackPtr->apiReporter->WriteEndEvent(API_SUCCESS, asyncCallbackInfo->err);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called end");
}

napi_value GetDeferredLink(napi_env env, napi_callback_info info)
{
    std::unique_ptr<Dfx::ApiEventReporter> reporter = std::make_unique<Dfx::ApiEventReporter>("GetDeferredLink");
    auto* asyncCallbackInfo = new (std::nothrow) GetDeferredLinkCallbackInfo(env, std::move(reporter));
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<GetDeferredLinkCallbackInfo> callbackPtr{ asyncCallbackInfo };
    size_t argc = 1;
    napi_value args[1] = { nullptr };
    NAPI_CALL_BASE(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr), nullptr);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, args[0], &valueType);
    if (valueType == napi_function) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "is callback");
        NAPI_CALL(env, napi_create_reference(env, args[0], 1, &asyncCallbackInfo->callback));
    }
    auto promise = AsyncCallNativeMethod<GetDeferredLinkCallbackInfo>(
        env, asyncCallbackInfo, GET_DEFERRED_LINK, GetDeferredLinkExec, GetDeferredLinkComplete);
    callbackPtr.release();
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "GetDeferredLink end");
    return promise;
}
}