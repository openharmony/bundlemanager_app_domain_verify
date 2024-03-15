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

#include "domain_verifier.h"
#include "app_domain_verify_hilog.h"
#include "domain_json_util.h"

namespace OHOS {
namespace AppDomainVerify {
InnerVerifyStatus DomainVerifier::VerifyHost(OHOS::NetStack::HttpClient::ResponseCode responseCode,
    const std::string &assetJsonsStr, const AppVerifyBaseInfo &appVerifyBaseInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    if (responseCode != OHOS::NetStack::HttpClient::ResponseCode::OK) {
        return GetVerifyStatusFromHttpError(responseCode);
    }
    AssetJsonObj assetJsonObj;
    if (JsonUtil::Parse(assetJsonsStr, assetJsonObj)) {
        InnerVerifyStatus status = VerifyHostWithAppIdentifier(assetJsonObj, appVerifyBaseInfo);
        if (status == InnerVerifyStatus::UNKNOWN) {
            return VerifyHostWithBundleName(assetJsonObj, appVerifyBaseInfo);
        } else {
            return status;
        }
    }
    return InnerVerifyStatus::STATE_FAIL;
}

InnerVerifyStatus DomainVerifier::GetVerifyStatusFromHttpError(OHOS::NetStack::HttpClient::ResponseCode responseCode)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    if (responseCode >= OHOS::NetStack::HttpClient::ResponseCode::MULT_CHOICE &&
        responseCode < OHOS::NetStack::HttpClient::ResponseCode::BAD_REQUEST) {
        // 3xx
        return InnerVerifyStatus::FAILURE_REDIRECT;
    }
    if (responseCode >= OHOS::NetStack::HttpClient::ResponseCode::BAD_REQUEST &&
        responseCode < OHOS::NetStack::HttpClient::ResponseCode::INTERNAL_ERROR) {
        // 4xx
        return InnerVerifyStatus::FAILURE_CLIENT_ERROR;
    }
    if (responseCode >= OHOS::NetStack::HttpClient::ResponseCode::INTERNAL_ERROR) {
        // 5xx
        return InnerVerifyStatus::FAILURE_REJECTED_BY_SERVER;
    }
    return InnerVerifyStatus::FAILURE_HTTP_UNKNOWN;
}

InnerVerifyStatus DomainVerifier::VerifyHostWithAppIdentifier(const AssetJsonObj &assetJsonObj,
    const AppVerifyBaseInfo &appVerifyBaseInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    if (appVerifyBaseInfo.appIdentifier.empty()) {
        return InnerVerifyStatus::UNKNOWN;
    }
    for (auto itr = assetJsonObj.applinking.apps.begin(); itr != assetJsonObj.applinking.apps.end(); ++itr) {
        if (itr->appIdentifier.empty()) {
            continue;
        }
        // if appIdentifier equals
        if (appVerifyBaseInfo.appIdentifier == itr->appIdentifier) {
            if (!appVerifyBaseInfo.bundleName.empty() && !itr->bundleName.empty() &&
                appVerifyBaseInfo.bundleName != itr->bundleName) {
                return InnerVerifyStatus::STATE_FAIL;
            }
            if (!appVerifyBaseInfo.fingerprint.empty() && !itr->fingerprint.empty() &&
                appVerifyBaseInfo.fingerprint != itr->fingerprint) {
                return InnerVerifyStatus::STATE_FAIL;
            }
            return InnerVerifyStatus::STATE_SUCCESS;
        }
        // if appIdentifier not equal, bundleName must not equal
        if (!appVerifyBaseInfo.bundleName.empty() && appVerifyBaseInfo.bundleName == itr->bundleName) {
            return InnerVerifyStatus::STATE_FAIL;
        }
    }
    return InnerVerifyStatus::UNKNOWN;
}

InnerVerifyStatus DomainVerifier::VerifyHostWithBundleName(const AssetJsonObj &assetJsonObj,
    const AppVerifyBaseInfo &appVerifyBaseInfo)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "%s called", __func__);
    if (appVerifyBaseInfo.bundleName.empty() || appVerifyBaseInfo.fingerprint.empty()) {
        return InnerVerifyStatus::STATE_FAIL;
    }

    for (auto itr = assetJsonObj.applinking.apps.begin(); itr != assetJsonObj.applinking.apps.end(); ++itr) {
        if (appVerifyBaseInfo.bundleName == itr->bundleName) {
            return appVerifyBaseInfo.fingerprint == itr->fingerprint ?
                InnerVerifyStatus::STATE_SUCCESS :
                InnerVerifyStatus::STATE_FAIL;
        }
    }
    return InnerVerifyStatus::STATE_FAIL;
}
}
}
