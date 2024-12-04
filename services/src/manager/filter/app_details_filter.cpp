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
#include "app_details_filter.h"
#include "skill.h"
#include "want.h"
#include "app_domain_verify_hilog.h"

namespace OHOS {
namespace AppDomainVerify {
constexpr const static char *ENTITY_SYSTEM_BROWSABLE = "entity.system.browsable";
constexpr const static char *WANT_ACTION_VIEWDATA = "ohos.want.action.viewData";
bool AppDetailsFilter::Filter(const std::vector<AppDetailInfo>& input, std::vector<AppDetailInfo>& dest, const std::string& url)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "call.");
    AAFwk::Want want;
    want.SetAction(WANT_ACTION_VIEWDATA);
    want.SetEntities({ENTITY_SYSTEM_BROWSABLE});
    want.SetUri(url);
    AppExecFwk::Skill skill;
    skill.actions.push_back(WANT_ACTION_VIEWDATA);
    skill.entities.push_back(ENTITY_SYSTEM_BROWSABLE);
    for (auto&info : input) {
        skill.uris.push_back(info.m_skillUri);
    }
    size_t index = 0;
    bool ret = skill.Match(want, index);
    if (!ret) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "skill match fail.");
        return false;
    }
    dest.clear();
    dest.push_back(input[index]);
    return true;
}
}
}
