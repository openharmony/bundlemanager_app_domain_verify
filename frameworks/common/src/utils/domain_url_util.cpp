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

#include <string>
#include "uri.h"
#include "domain_url_util.h"
namespace OHOS {
namespace AppDomainVerify {
const int HOST_PATTERN_MATCH_SIZE = 3;
const int SCHEME_PATTERN_MATCH_SIZE = 2;
static const std::string SCHEMA_HTTPS = "https";
bool UrlUtil::IsValidAppDomainVerifyHost(const std::string& host)
{
    return true;
}
bool UrlUtil::IsValidUrl(const std::string& url)
{
    if (url.empty()) {
        return false;
    }
    Uri uri(url);
    return (uri.GetScheme() == SCHEMA_HTTPS) && (!uri.GetHost().empty());
}
std::string UrlUtil::GetHost(const std::string& url)
{
    Uri uri(url);
    return uri.GetHost();
}
std::string UrlUtil::GetScheme(const std::string& url)
{
    Uri uri(url);
    return uri.GetScheme();
}
}
}