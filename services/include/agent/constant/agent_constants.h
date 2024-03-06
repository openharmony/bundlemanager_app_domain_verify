/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef APP_DOMAIN_VERIFY_AGENT_CONSTANTS_H
#define APP_DOMAIN_VERIFY_AGENT_CONSTANTS_H

#include <string>

namespace OHOS {
namespace AppDomainVerify {
namespace ApplinkingAssetKeys {
const std::string APP_LINKING = "applinking";
const std::string APPS = "apps";
const std::string BUNDLE_NAME = "bundleName";
const std::string APP_IDENTIFIER = "appIdentifier";
const std::string FINGERPRINT = "fingerprint";
const std::string ASSET_PATH = "/.well-known/";
const std::string ASSET_NAME = "applinking.json";
}  // namespace ApplinkingAssetKeys
}  // namespace AppDomainVerify
}  // namespace OHOS
#endif  // APP_DOMAIN_VERIFY_AGENT_CONSTANTS_H