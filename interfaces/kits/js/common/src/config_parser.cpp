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
#include "config_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include "app_domain_verify_hilog.h"

namespace OHOS::AppDomainVerify::Dfx {
constexpr int MAX_FILE_SIZE = 1024;
constexpr int MAX_LINE_SIZE = 1024;

bool ConfigParser::load(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        APP_DOMAIN_VERIFY_HILOGE(
            APP_DOMAIN_VERIFY_MODULE_JS_NAPI, "Unable to open file: filename:%{public}s", filename.c_str());
        return false;
    }

    if (file.tellg() > MAX_FILE_SIZE) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_JS_NAPI, "File size exceeds maximum allowed size");
        return false;
    }

    file.seekg(0, std::ios::beg);
    std::string line;
    while (std::getline(file, line)) {
        if (line.size() > MAX_LINE_SIZE) {
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_JS_NAPI, "Line size exceeds maximum allowed size");
            return false;
        }
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;  // Skip comments and empty lines
        }

        auto delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = trim(line.substr(0, delimiterPos));
            std::string value = trim(line.substr(delimiterPos + 1));
            configData[key] = value;
        }
    }
    return true;
}

std::string ConfigParser::get(const std::string& key, const std::string& defaultValue) const
{
    auto it = configData.find(key);
    if (it != configData.end()) {
        return it->second;
    }
    return defaultValue;
}

std::string ConfigParser::trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

}