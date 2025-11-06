/*
 * Copyright (C) 2023-2025 Huawei Device Co., Ltd.
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
#include "cJSON.h"
#include "domain_json_util.h"
#include "agent_constants.h"
#include "app_domain_verify_hilog.h"

namespace OHOS {
namespace AppDomainVerify {

void JsonUtil::ParseItem(AssetJsonObj& assetJsonObj, cJSON* appsArray)
{
    int arraySize = cJSON_GetArraySize(appsArray);
    for (int i = 0; i < arraySize; i++) {
        cJSON* arrayItem = cJSON_GetArrayItem(appsArray, i);
        if (!cJSON_IsObject(arrayItem)) {
            continue;
        }
        AppVerifyBaseInfo appVerifyBaseInfo;
        cJSON* appIdentifier = cJSON_GetObjectItemCaseSensitive(arrayItem, ApplinkingAssetKeys::APP_IDENTIFIER.c_str());
        if (appIdentifier != nullptr && cJSON_IsString(appIdentifier) && appIdentifier->valuestring != nullptr) {
            appVerifyBaseInfo.appIdentifier = appIdentifier->valuestring;
        }
        cJSON* bundleName = cJSON_GetObjectItemCaseSensitive(arrayItem, ApplinkingAssetKeys::BUNDLE_NAME.c_str());
        if (bundleName != nullptr && cJSON_IsString(bundleName) && bundleName->valuestring != nullptr) {
            appVerifyBaseInfo.bundleName = bundleName->valuestring;
        }
        cJSON* fingerprint = cJSON_GetObjectItemCaseSensitive(arrayItem, ApplinkingAssetKeys::FINGERPRINT.c_str());
        if (fingerprint != nullptr && cJSON_IsString(fingerprint) && fingerprint->valuestring != nullptr) {
            appVerifyBaseInfo.fingerprint = fingerprint->valuestring;
        }
        cJSON* priority = cJSON_GetObjectItemCaseSensitive(arrayItem, ApplinkingAssetKeys::PRIORITY.c_str());
        if (priority != nullptr && cJSON_IsNumber(priority)) {
            appVerifyBaseInfo.priority = priority->valueint;
        }
        assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfo);
    }
}
bool JsonUtil::Parse(const std::string &assetJsonsStr, AssetJsonObj &assetJsonObj)
{
    if (assetJsonsStr.empty()) {
        return false;
    }
    cJSON *jsonObj = cJSON_Parse(assetJsonsStr.c_str());
    if (jsonObj == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "assetJsonsStr can not be parsed.");
        return false;
    }
    if (!cJSON_IsObject(jsonObj)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "assetLinksStr can not be parsed into obj.");
        cJSON_Delete(jsonObj);
        return false;
    }
    cJSON *applinkingObj = cJSON_GetObjectItemCaseSensitive(jsonObj, ApplinkingAssetKeys::APP_LINKING.c_str());
    if (applinkingObj == nullptr || !cJSON_IsObject(applinkingObj)) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "can not parsed applinking into obj.");
        cJSON_Delete(jsonObj);
        return false;
    }
    cJSON *appsArray = cJSON_GetObjectItemCaseSensitive(applinkingObj, ApplinkingAssetKeys::APPS.c_str());
    if (appsArray != nullptr && cJSON_IsArray(appsArray)) {
        ParseItem(assetJsonObj, appsArray);
        cJSON_Delete(jsonObj);
        return true;
    }
    cJSON_Delete(jsonObj);
    return false;
}
}
}
