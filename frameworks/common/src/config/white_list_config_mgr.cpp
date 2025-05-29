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
#include <sstream>
#include <algorithm>
#include "domain_url_util.h"
#include "white_list_config_mgr.h"
#include "app_domain_verify_hilog.h"
#include "app_domain_verify_hisysevent.h"
#include "regex.h"
#include "uri.h"

namespace OHOS::AppDomainVerify {
const static std::string DYNAMIC_WHITE_LIST_PRE_PATH =
    "/data/service/el1/public/app_domain_verify_mgr_service/whitelist_pref";
const static std::string DEFAULT_WHITE_LIST_PRE_PATH = "/system/etc/app_domain_verify/whitelist_pref";
const static std::string REG_DEFAULT_URL_KEY = "regDefaultUrl";
const static std::string REG_WHITE_LIST_KEY = "regWhiteList";
const static std::string SPLITOR = ",";
constexpr int REG_ERR_BUF = 1024;
constexpr int NM = 10;
WhiteListConfigMgr::WhiteListConfigMgr()
{
    Load();
}
WhiteListConfigMgr::~WhiteListConfigMgr()
{
}
void WhiteListConfigMgr::LoadDefault()
{
    preferences_ = GetPreference(DEFAULT_WHITE_LIST_PRE_PATH);
    if (preferences_ == nullptr) {
        UNIVERSAL_ERROR_EVENT(READ_DEFAULT_WHITE_LIST_FAULT);
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "WhiteListConfigMgr::Load failed.");
        return;
    }

    defaultWhiteUrl_ = preferences_->GetString(REG_DEFAULT_URL_KEY, "");
    if (defaultWhiteUrl_.empty()) {
        UNIVERSAL_ERROR_EVENT(READ_DEFAULT_WHITE_LIST_FAULT);
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MODULE_COMMON, "WhiteListConfigMgr::Load defaultWhiteUrl empty.");
    }
}
void WhiteListConfigMgr::LoadDynamic()
{
    preferences_ = GetPreference(DYNAMIC_WHITE_LIST_PRE_PATH);
    if (preferences_ == nullptr) {
        UNIVERSAL_ERROR_EVENT(READ_DYNAMIC_WHITE_LIST_FAULT);
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "WhiteListConfigMgr::Load failed.");
        return;
    }

    auto whiteListStr = preferences_->GetString(REG_WHITE_LIST_KEY, "");
    Split(whiteListStr);
}
void WhiteListConfigMgr::Load()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "called");
    LoadDefault();
    LoadDynamic();
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "called end");
}
void WhiteListConfigMgr::Split(std::string src)
{
    whiteListSet_.clear();
    if (!SPLITOR.empty()) {
        size_t pos = 0;
        while ((pos = src.find(SPLITOR)) != std::string::npos) {
            // split
            std::string token = src.substr(0, pos);
            if (!token.empty()) {
                whiteListSet_.insert(token);
            }
            src.erase(0, pos + SPLITOR.length());
        }
    }

    if (!src.empty()) {
        whiteListSet_.insert(src);
    }
    if (whiteListSet_.empty()) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MODULE_COMMON, "WhiteListConfigMgr::Split whiteListSet empty.");
    }
}
std::shared_ptr<NativePreferences::Preferences> WhiteListConfigMgr::GetPreference(const std::string& path)
{
    int status;
    NativePreferences::Options options(path);
    std::shared_ptr<NativePreferences::Preferences> preferences = NativePreferences::PreferencesHelper::GetPreferences(
        options, status);
    if (status != 0) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "WhiteListConfigMgr::GetPreference failed.");
        return nullptr;
    }
    return preferences;
}
bool WhiteListConfigMgr::Save()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "called");
    if (preferences_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MODULE_COMMON, "preferences null");
        return false;
    }
    std::stringstream strSteam;
    for (const auto& element : whiteListSet_) {
        strSteam << element << ",";
    }
    auto ret = preferences_->PutString(REG_WHITE_LIST_KEY, strSteam.str());
    if (ret != 0) {
        UNIVERSAL_ERROR_EVENT(WRITE_DYNAMIC_WHITE_LIST_FAULT);
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "put dynamic white list error ret:%{public}d.", ret);
        return false;
    }
    preferences_->Flush();
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "WhiteListConfigMgr::Save %{public}s ret%{public}d.",
        strSteam.str().c_str(), ret);
    return true;
}

bool WhiteListConfigMgr::IsInWhiteList(const std::string& url)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called");
    std::lock_guard<std::mutex> lock(whiteListLock_);
    bool ret;
    if (whiteListSet_.empty()) {
        ret = IsMatched(url, defaultWhiteUrl_);
    } else {
        auto targetPatten = std::find_if(whiteListSet_.begin(), whiteListSet_.end(),
            [&url](const std::string& urlPatten) { return IsMatched(url, urlPatten); });

        ret = (targetPatten != whiteListSet_.end()) || (IsMatched(url, defaultWhiteUrl_));
    }
    APP_DOMAIN_VERIFY_HILOGI(
        APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "is matched url %{private}s, ret:%{public}d", url.c_str(), ret);
    return ret;
}
void WhiteListConfigMgr::UpdateWhiteList(const std::unordered_set<std::string>& whiteList)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called 1");
    std::unordered_set<std::string> filtedWhiteList;
    std::for_each(whiteList.begin(), whiteList.end(), [&filtedWhiteList](const std::string& element) {
        if (!element.empty()) {
            filtedWhiteList.insert(element);
        }
    });
    std::lock_guard<std::mutex> lock(whiteListLock_);
    whiteListSet_ = filtedWhiteList;
    if (!whiteListSet_.empty()) {
        if (!Save()) {
            UNIVERSAL_ERROR_EVENT(WRITE_DYNAMIC_WHITE_LIST_FAULT);
            APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "save white list failed.");
        }
    }
}

bool WhiteListConfigMgr::IsMatched(const std::string& url, const std::string& regPatten)
{
    if (regPatten.empty()) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "regPatten null");
        return false;
    }
    const char* bematch = url.c_str();
    char errbuf[REG_ERR_BUF];
    regex_t reg;
    int errNum = 0;
    int nm = NM;
    regmatch_t pmatch[nm];
    if (regcomp(&reg, regPatten.c_str(), REG_EXTENDED) < 0) {
        regerror(errNum, &reg, errbuf, sizeof(errbuf));
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "regexec error:%{public}s", errbuf);
        return false;
    }
    errNum = regexec(&reg, bematch, nm, pmatch, 0);
    if (errNum == REG_NOMATCH) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "regexec no match");
        regfree(&reg);
        return false;
    } else if (errNum) {
        regerror(errNum, &reg, errbuf, sizeof(errbuf));
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "regexec error:%{public}s", errbuf);
        regfree(&reg);
        return false;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "is valid path");
    regfree(&reg);
    return true;
}

}
