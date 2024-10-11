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
#ifndef APP_DOMAIN_VERIFY_MANAGER_CONFIG_PARSER
#define APP_DOMAIN_VERIFY_MANAGER_CONFIG_PARSER

#include <map>
#include <string>

namespace OHOS::AppDomainVerify::Dfx {

class ConfigParser {
public:
    bool load(const std::string& filename);

    std::string get(const std::string& key, const std::string& defaultValue = "") const;

private:
    static std::string trim(const std::string& str);

    std::map<std::string, std::string> configData;
};

}
#endif  // APP_DOMAIN_VERIFY_MANAGER_CONFIG_PARSER