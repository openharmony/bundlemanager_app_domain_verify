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
#include "white_list_updater.h"
#include "app_domain_verify_hilog.h"
#include "domain_url_util.h"
namespace OHOS::AppDomainVerify {

void WhiteListUpdater::UpdateWhiteList(std::unordered_set<std::string>& whiteList)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    if (!init) {
        Load();
    }
    std::unordered_set<std::string> filtedWhiteList;
    std::for_each(whiteList.begin(), whiteList.end(), [&filtedWhiteList](const std::string& element){
        if(UrlUtil::IsValidUrl(element)){
            filtedWhiteList.insert(element);
        }
    });
    std::lock_guard<std::mutex> lock(whiteListLock);
    whiteListSet_ = filtedWhiteList;
    if (!whiteListSet_.empty()) {
        Save();
    }
}
}