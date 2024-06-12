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
#ifndef APP_DOMAIN_VERIFY_WHITELISTCONFIGMANAGER_H
#define APP_DOMAIN_VERIFY_WHITELISTCONFIGMANAGER_H
#include <string>
#include <unordered_set>
#include <vector>
#include <atomic>
#include "singleton.h"
#include "preferences.h"
#include "preferences_helper.h"
namespace OHOS::AppDomainVerify {
class WhiteListConfigMgr;
class PreferencesObserverUpdater : public NativePreferences::PreferencesObserver {
public:
    explicit PreferencesObserverUpdater(WhiteListConfigMgr* mgr);
    ~PreferencesObserverUpdater() override;
    void OnChange(const std::string& key) override;

private:
    WhiteListConfigMgr* mgr_;
};
class WhiteListConfigMgr {
public:
    explicit WhiteListConfigMgr(bool needObserve = false);
    virtual ~WhiteListConfigMgr();

protected:
    void Load();
    void Reload();
    bool Save();
    bool OnUpdate();
    void Split(std::string urlList);
    std::shared_ptr<NativePreferences::Preferences> GetPreference();
    std::string defaultWhiteUrl_;
    std::unordered_set<std::string> whiteListSet_;
    std::shared_ptr<NativePreferences::Preferences> preferences_;
    std::mutex initLock;
    std::atomic<bool> init = false;
    std::shared_ptr<PreferencesObserverUpdater> observer_;
    bool needObserve_;
    friend class PreferencesObserverUpdater;
};

}

#endif  // APP_DOMAIN_VERIFY_WHITELISTCONFIGMANAGER_H
