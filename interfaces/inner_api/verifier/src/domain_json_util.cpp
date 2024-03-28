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
#include "json.hpp"
#include "domain_json_util.h"
#include "agent_constants.h"
#include "app_domain_verify_hilog.h"

namespace OHOS {
namespace AppDomainVerify {
using json = nlohmann::json;

bool JsonUtil::Parse(const std::string &assetJsonsStr, AssetJsonObj &assetJsonObj)
{
    if (!assetJsonsStr.empty()) {
        json jsonObj;
        try {
            jsonObj = json::parse(assetJsonsStr);
        } catch (json::parse_error &e) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "assetJsonsStr can not be parsed.");
            return false;
        }
        if (jsonObj == nullptr || !jsonObj.is_object()) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE,
                "assetLinksStr can not be parsed into obj.");
            return false;
        }
        if (jsonObj.find(ApplinkingAssetKeys::APP_LINKING) == jsonObj.end() ||
            !jsonObj.at(ApplinkingAssetKeys::APP_LINKING).is_object()) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "can not parsed applinking into obj.");
            return false;
        }
        auto applinkingObj = jsonObj.at(ApplinkingAssetKeys::APP_LINKING);
        if (applinkingObj.find(ApplinkingAssetKeys::APPS) != applinkingObj.end() &&
            applinkingObj.at(ApplinkingAssetKeys::APPS).is_array()) {
            auto appsArray = applinkingObj.at(ApplinkingAssetKeys::APPS);
            for (size_t i = 0; i < appsArray.size(); i++) {
                AppVerifyBaseInfo appVerifyBaseInfo;
                auto arrayItem = appsArray[i];
                appVerifyBaseInfo.appIdentifier = arrayItem.find(ApplinkingAssetKeys::APP_IDENTIFIER) !=
                            arrayItem.end() &&
                        arrayItem.at(ApplinkingAssetKeys::APP_IDENTIFIER).is_string() ?
                    arrayItem.at(ApplinkingAssetKeys::APP_IDENTIFIER) :
                    "";
                appVerifyBaseInfo.bundleName = arrayItem.find(ApplinkingAssetKeys::BUNDLE_NAME) != arrayItem.end() &&
                        arrayItem.at(ApplinkingAssetKeys::BUNDLE_NAME).is_string() ?
                    arrayItem.at(ApplinkingAssetKeys::BUNDLE_NAME) :
                    "";
                appVerifyBaseInfo.fingerprint = arrayItem.find(ApplinkingAssetKeys::FINGERPRINT) != arrayItem.end() &&
                        arrayItem.at(ApplinkingAssetKeys::FINGERPRINT).is_string() ?
                    arrayItem.at(ApplinkingAssetKeys::FINGERPRINT) :
                    "";
                assetJsonObj.applinking.apps.emplace_back(appVerifyBaseInfo);
            }
            return true;
        }
    }
    return false;
}
}
}
