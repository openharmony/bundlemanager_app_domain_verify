/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef APP_DOMAIN_VERIFY_DEFERRED_LINK_MGR_H
#define APP_DOMAIN_VERIFY_DEFERRED_LINK_MGR_H
#include <string>
#include <mutex>
#include <set>
#include "datetime_ex.h"
#include "uri.h"
#include "event_handler.h"
#include "event_runner.h"
#include "app_domain_verify_hilog.h"
#include "ability_filter.h"

namespace OHOS::AppDomainVerify {

struct DeferredLinkInfo {
    std::string domain;
    std::string url;
    int64_t timeStamp;
};
/**
 * DeferredLinkMgr
 * @descrition
 * The mgr keeps limited links which are generated when the link is clicked but app is not installed.
 * After app installed, app can get the link which click to open it, and app can continue the browsing.
 */
class DeferredLinkMgr {
public:
    DeferredLinkMgr();
    /**
     * PutDeferredLink
     * @descrition put deferred link info
     * @param domain the domain of url.
     * @param url the url to open.
     */
    void PutDeferredLink(const DeferredLinkInfo& info);

    /**
     * GetDeferredLink
     * @descrition get deferred link within domains and bundleName
     * @param domains the domains to filter link info.
     * @return url the deferred url to open.
     */
    std::string GetDeferredLink(const std::string& bundleName, const std::vector<std::string>& domains);

    ~DeferredLinkMgr();

private:
    bool CanMatchAbility(const std::string& bundleName, const std::string& url);
    void PostAgeCacheTask();
    void AgeCacheProcess();
    void CheckStartTimerUnlocked();
    void CheckFullUnlocked(const DeferredLinkInfo& info);
    void CheckRemoveExistedUnlocked(const DeferredLinkInfo& info);
    /**
     * cache list
     * @descrition list contains deferred link info, newly in front, older in back.
     */
    std::list<DeferredLinkInfo> caches;
    std::mutex cachesMutex_;
    std::shared_ptr<AppExecFwk::EventHandler> ageHandler_;
    std::shared_ptr<AppExecFwk::EventRunner> ageRunner_;
    std::shared_ptr<AbilityFilter> abilityFilter_ = AbilityFilter::Create();
};
}
#endif  // APP_DOMAIN_VERIFY_DEFERRED_LINK_MGR_H
