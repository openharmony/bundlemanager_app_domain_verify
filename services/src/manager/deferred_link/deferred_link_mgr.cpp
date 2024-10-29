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
#include "deferred_link_mgr.h"

namespace OHOS::AppDomainVerify {
namespace {
constexpr const char* TASK_ID = "age";
constexpr int32_t DELAY_TIME = 60000;        // 1min
constexpr int64_t MAX_CACHE_TIME = 600;  // 10min
constexpr int MAX_CACHE_SIZE = 50;
}
void DeferredLinkMgr::PutDeferredLink(const DeferredLinkInfo& info)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called.");
    std::unique_lock<std::mutex> lock(cachesMutex_);
    CheckStartTimerUnlocked();
    CheckRemoveExistedUnlocked(info);
    CheckFullUnlocked(info);

    caches_.push_front(info);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "info domain:%{private}s, url:%{private}s.",
        info.domain.c_str(), info.url.c_str());
}

std::string DeferredLinkMgr::GetDeferredLink(const std::string& bundleName, const std::vector<std::string>& domains)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called.");

    std::set<std::string> domainSet(domains.begin(), domains.end());
    std::unique_lock<std::mutex> lock(cachesMutex_);
    std::list<DeferredLinkInfo> destination;
    // find links in bundle's domain and can match bundle's ability, then remove all of them.
    caches_.remove_if([this, &bundleName, &domainSet, &destination](const DeferredLinkInfo& linkInfo) {
        if (domainSet.count(linkInfo.domain) != 0 && CanMatchAbility(bundleName, linkInfo.url)) {
            // keep newly in front
            destination.push_back(linkInfo);
            APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "matched.");
            return true;
        }
        return false;
    });

    std::string result = destination.empty() ? "" : destination.front().url;
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "get deferred url:%{private}s", result.c_str());
    return result;
}

bool DeferredLinkMgr::CanMatchAbility(const std::string& bundleName, const std::string& url)
{
    return abilityFilter_->Filter({ .bundleName = bundleName, .url = url });
}
void DeferredLinkMgr::PostAgeCacheTask()
{
    if (ageHandler_) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "PostAgeCacheTask.");
        ageHandler_->PostTask([this] { AgeCacheProcess(); }, TASK_ID, DELAY_TIME);
    }
}
void DeferredLinkMgr::AgeCacheProcess()
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "age func in.");
    std::unique_lock<std::mutex> lock(cachesMutex_);
    int64_t now = GetSecondsSince1970ToNow();
    caches_.remove_if([now](const DeferredLinkInfo& linkInfo) {
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE,
            "url:%{public}s, now:%{public}lld, timestamp%{public}lld.", linkInfo.url.c_str(), now, linkInfo.timeStamp);
        return now - linkInfo.timeStamp >= MAX_CACHE_TIME;
    });
    if (!caches_.empty()) {
        APP_DOMAIN_VERIFY_HILOGD(
            APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "post continue age task, remain size:%{public}zu.", caches_.size());
        PostAgeCacheTask();
    } else {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "age task end.");
    }
}
void DeferredLinkMgr::CheckStartTimerUnlocked()
{
    if (caches_.empty()) {
        PostAgeCacheTask();
    }
}
void DeferredLinkMgr::CheckFullUnlocked(const DeferredLinkInfo& info)
{
    if (caches_.size() == MAX_CACHE_SIZE) {
        caches_.pop_back();
    }
}
void DeferredLinkMgr::CheckRemoveExistedUnlocked(const DeferredLinkInfo& info)
{
    caches_.remove_if([&info](const DeferredLinkInfo& curInfo) {
        if (curInfo.url == info.url && curInfo.domain == info.domain) {
            return true;
        }
        return false;
    });
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "called.");
}
DeferredLinkMgr::DeferredLinkMgr()
{
    ageRunner_ = AppExecFwk::EventRunner::Create("age_deferred_link", AppExecFwk::ThreadMode::FFRT);
    if (ageRunner_ == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "create runner failed.");
        return;
    }
    ageHandler_ = std::make_shared<AppExecFwk::EventHandler>(ageRunner_);
}

DeferredLinkMgr::~DeferredLinkMgr()
{
    if (ageHandler_) {
        ageHandler_->RemoveAllEvents();
    }
}
}