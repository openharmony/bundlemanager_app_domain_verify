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
#include "white_list_config_mgr.h"
#include "app_domain_verify_hilog.h"

namespace OHOS::AppDomainVerify {
const static std::string DYNAMIC_WHITE_LIST_PRE_PATH =
    "/data/service/el1/public/app_domain_verify_mgr_service/whitelist_pref";
const static std::string DEFAULT_WHITE_LIST_PRE_PATH = "/system/etc/app_domain_verify/whitelist_pref";
const static std::string DEFAULT_URL_KEY = "defaultUrl";
const static std::string WHITE_LIST_KEY = "whiteList";
const static std::string SPLITOR = ",";
WhiteListConfigMgr::WhiteListConfigMgr(bool needObserve)
{
    needObserve_ = needObserve;
    Load();
}
WhiteListConfigMgr::~WhiteListConfigMgr()
{
    if (preferences_ && needObserve_) {
        preferences_->UnRegisterDataObserver(observer_);
    }
}
void WhiteListConfigMgr::LoadDefault()
{
    preferences_ = GetPreference(DEFAULT_WHITE_LIST_PRE_PATH);
    if (preferences_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "WhiteListConfigMgr::Load failed.");
        return;
    }

    defaultWhiteUrl_ = preferences_->GetString(DEFAULT_URL_KEY, "");
    if (defaultWhiteUrl_.empty()) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MODULE_COMMON, "WhiteListConfigMgr::Load defaultWhiteUrl empty.");
    }
}
void WhiteListConfigMgr::LoadDynamic()
{
    preferences_ = GetPreference(DYNAMIC_WHITE_LIST_PRE_PATH);
    if (preferences_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "WhiteListConfigMgr::Load failed.");
        return;
    }
    observer_ = std::make_shared<PreferencesObserverUpdater>(this);
    if (needObserve_) {
        preferences_->RegisterObserver(observer_, NativePreferences::RegisterMode::MULTI_PRECESS_CHANGE);
    }

    auto whiteListStr = preferences_->GetString(WHITE_LIST_KEY, "");
    Split(whiteListStr);
}
void WhiteListConfigMgr::Load()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "%s called", __func__);
    std::lock_guard<std::mutex> lock(initLock);
    LoadDefault();
    LoadDynamic();

    init = true;
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "%s called end", __func__);
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
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "%s called", __func__);
    if (preferences_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGW(APP_DOMAIN_VERIFY_MODULE_COMMON, "preferences null");
        return false;
    }
    std::stringstream strSteam;
    for (const auto& element : whiteListSet_) {
        strSteam << element << ",";
    }
    auto ret = preferences_->PutString(WHITE_LIST_KEY, strSteam.str());
    preferences_->Flush();
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "WhiteListConfigMgr::Save %{public}s ret%{public}d.",
        strSteam.str().c_str(), ret);
    return true;
}
bool WhiteListConfigMgr::OnUpdate()
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "%s called", __func__);
    if (preferences_ == nullptr) {
        return false;
    }
    NativePreferences::PreferencesHelper::RemovePreferencesFromCache(DYNAMIC_WHITE_LIST_PRE_PATH);
    Reload();
    if (preferences_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnUpdate relaod preferences failed");
        return false;
    }
    auto whiteListStr = preferences_->GetString(WHITE_LIST_KEY, "");
    Split(whiteListStr);
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "%s called end", __func__);
    return true;
}
void WhiteListConfigMgr::Reload()
{
    init = false;
    Load();
}

void PreferencesObserverUpdater::OnChange(const std::string& key)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "OnChange key:%{public}s.", key.c_str());
    if (mgr_) {
        mgr_->OnUpdate();
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MODULE_COMMON, "%s called end", __func__);
}
PreferencesObserverUpdater::PreferencesObserverUpdater(WhiteListConfigMgr* mgr)
{
    mgr_ = mgr;
}
PreferencesObserverUpdater::~PreferencesObserverUpdater()
{
    mgr_ = nullptr;
}
}
