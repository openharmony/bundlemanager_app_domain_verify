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
#include "white_list_checker.h"
#include "app_domain_verify_hilog.h"

namespace OHOS::AppDomainVerify {

bool WhiteListChecker::IsInWhiteList(const std::string& url)
{
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "%s called", __func__);
    if (!init) {
        Load();
    }
    bool ret;
    if (whiteListSet_.empty()) {
        ret = (url == defaultWhiteUrl_);
    } else {
        ret = (whiteListSet_.count(url) != 0) || (url == defaultWhiteUrl_);
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "is count %{public}d url %{public}s",
        whiteListSet_.count(url) != 0, url.c_str());
    return ret;
}
WhiteListChecker::WhiteListChecker() : WhiteListConfigMgr(true)
{
}
}