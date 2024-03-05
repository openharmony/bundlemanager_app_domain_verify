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

#include <regex>
#include <string>
#include "domain_url_util.h"
namespace OHOS {
namespace AppDomainVerify {
const std::regex URL_PATTERN(R"(^(https?)://[^\s/$.?#].[^\s]*$)");
const std::regex HOST_PATTERN(R"(^(https?)://([^/\r\n]+)(/[^\r\n]*)?$)");
const std::regex SCHEME_PATTERN(R"(^(https?)://)");
const int HOST_PATTERN_MATCH_SIZE = 3;
const int SCHEME_PATTERN_MATCH_SIZE = 2;

bool UrlUtil::IsValidAppDomainVerifyHost(const std::string &host)
{
    return true;
}
bool UrlUtil::IsValidUrl(const std::string &url)
{
    return std::regex_match(url, URL_PATTERN);
}
std::string UrlUtil::GetHost(const std::string &url)
{
    std::smatch match;
    if (std::regex_match(url, match, HOST_PATTERN)) {
        return match.size() >= HOST_PATTERN_MATCH_SIZE ? match[HOST_PATTERN_MATCH_SIZE - 1].str() : "";
    }
    return "";
}
std::string UrlUtil::GetScheme(const std::string &url)
{
    std::smatch match;
    if (std::regex_search(url, match, SCHEME_PATTERN)) {
        return match.size() >= SCHEME_PATTERN_MATCH_SIZE ? match[SCHEME_PATTERN_MATCH_SIZE - 1].str() : "";
    }
    return "";
}
}
}