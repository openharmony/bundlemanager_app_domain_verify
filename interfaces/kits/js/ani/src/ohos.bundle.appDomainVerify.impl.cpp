/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "ohos.bundle.appDomainVerify.proj.hpp"
#include "ohos.bundle.appDomainVerify.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include <map>
#include "app_domain_verify_hilog.h"
#include "app_domain_verify_mgr_client.h"
#include "js_common_defined.h"
#include "api_event_reporter.h"
using namespace taihe;
using namespace OHOS::AppDomainVerify;
namespace {
bool CheckInput(const std::string& input)
{
    if (input.empty() || input.size() > MAX_STR_INPUT_SIZE) {
        return false;
    }
    return true;
}
array<string> BuildError(ErrorCode errorCode)
{
    taihe::set_business_error(errorCode, ErrCodeMap[errorCode]);
    return taihe::array<string>(nullptr, 0);
}
array<string> BuildStringArray(const std::vector<std::string>& data)
{
    array<string> resArr(data.size(), "");
    for (int i = 0; i < data.size(); i++) {
        resArr[i] = data[i];
    }
    return resArr;
}
array<string> queryAssociatedDomains(string_view bundleName)
{
    Dfx::ApiEventReporter reporter("QueryAssociatedDomains");
    if (!CheckInput(bundleName.c_str())) {
        reporter.WriteEndEvent(Dfx::API_FAIL, ErrorCode::E_PARAM_ERROR);
        return BuildError(ErrorCode::E_PARAM_ERROR);
    }
    std::vector<std::string> domains;

    auto ret = AppDomainVerifyMgrClient::GetInstance()->QueryAssociatedDomains(bundleName.c_str(), domains);
    if (ret != 0) {
        if (ErrCodeMap.count(static_cast<ErrorCode>(ret)) != 0) {
            reporter.WriteEndEvent(Dfx::API_FAIL, static_cast<ErrorCode>(ret));
            return BuildError(static_cast<ErrorCode>(ret));
        } else {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "unknown error:%{public}d.", ret);
            reporter.WriteEndEvent(Dfx::API_FAIL, ret);
            return BuildStringArray(domains);
        }
    }
    reporter.WriteEndEvent(Dfx::API_SUCCESS, ret);

    return BuildStringArray(domains);
}

array<string> queryAssociatedBundleNames(string_view domain)
{
    Dfx::ApiEventReporter reporter("QueryAssociatedBundleNames");
    if (!CheckInput(domain.c_str())) {
        reporter.WriteEndEvent(Dfx::API_FAIL, ErrorCode::E_PARAM_ERROR);
        return BuildError(ErrorCode::E_PARAM_ERROR);
    }
    std::vector<std::string> bundleNames;

    auto ret = AppDomainVerifyMgrClient::GetInstance()->QueryAssociatedBundleNames(domain.c_str(), bundleNames);
    if (ret != 0) {
        if (ErrCodeMap.count(static_cast<ErrorCode>(ret)) != 0) {
            reporter.WriteEndEvent(Dfx::API_FAIL, static_cast<ErrorCode>(ret));
            return BuildError(static_cast<ErrorCode>(ret));
        } else {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "unknown error:%{public}d.", ret);
            reporter.WriteEndEvent(Dfx::API_FAIL, ret);
            return BuildStringArray(bundleNames);
        }
    }
    reporter.WriteEndEvent(Dfx::API_SUCCESS, ret);

    return BuildStringArray(bundleNames);
}
}  // namespace

TH_EXPORT_CPP_API_queryAssociatedDomains(queryAssociatedDomains);
TH_EXPORT_CPP_API_queryAssociatedBundleNames(queryAssociatedBundleNames);
