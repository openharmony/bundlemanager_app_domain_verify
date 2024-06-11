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
#ifndef APP_DOMAIN_VERIFIER_H
#define APP_DOMAIN_VERIFIER_H

#include "http_client.h"
#include "inner_verify_status.h"
#include "app_verify_base_info.h"
#include "asset_json_obj.h"
#include "skill_uri.h"

namespace OHOS {
namespace AppDomainVerify {
class DomainVerifier {
public:
    static InnerVerifyStatus VerifyHost(OHOS::NetStack::HttpClient::ResponseCode responseCode,
        const std::string &assetJsonsStr, const AppVerifyBaseInfo &appVerifyBaseInfo);

private:
    static InnerVerifyStatus GetVerifyStatusFromHttpError(OHOS::NetStack::HttpClient::ResponseCode responseCode);
    static InnerVerifyStatus VerifyHostWithAppIdentifier(const AssetJsonObj &assetJsonObj,
        const AppVerifyBaseInfo &appVerifyBaseInfo);
    static InnerVerifyStatus VerifyHostWithBundleName(const AssetJsonObj &assetJsonObj,
        const AppVerifyBaseInfo &appVerifyBaseInfo);
};
}
}
#endif