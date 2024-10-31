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
#include <string>
#include <utility>
#include "ability_filter.h"
#include "bundle_info_query.h"
#include "app_domain_verify_hilog.h"

namespace OHOS::AppDomainVerify {
using Skill = OHOS::AppExecFwk::Skill;

class BundleAbilityFilterImpl : public AbilityFilter {
public:
    explicit BundleAbilityFilterImpl(std::string bundleName) : bundleName_(std::move(bundleName))
    {
        Init();
    }
    bool Filter(const FilterInfo& info) override
    {
        Want urlWant;
        urlWant.SetUri(info.url);
        urlWant.AddEntity(ENTITY_BROWSER);
        urlWant.SetAction(ACTION_VIEW_DATA);

        auto matchedAbility = std::find_if(
            abilityInfos_.cbegin(), abilityInfos_.cend(), [&urlWant](const AbilityInfo& abilityInfo) {
                APP_DOMAIN_VERIFY_HILOGD(
                    APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "abilityInfo:%{private}s", abilityInfo.name.c_str());
                auto skills = abilityInfo.skills;
                auto matchedSkill = std::find_if(skills.cbegin(), skills.cend(), [&urlWant](const Skill& skill) {
                    if (skill.domainVerify && skill.Match(urlWant)) {
                        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "skill matched");
                        return true;
                    }
                    return false;
                });
                return matchedSkill != skills.cend();
            });

        return matchedAbility != abilityInfos_.cend();
    }
    void Init()
    {
        BundleInfoQuery::GetBundleAbilityInfos(bundleName_, abilityInfos_);
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "abilityInfos size:%zu", abilityInfos_.size());
    }

private:
    std::vector<AbilityInfo> abilityInfos_;
    std::string bundleName_;
};
std::shared_ptr<AbilityFilter> AbilityFilter::Create(const std::string& bundleName)
{
    return std::make_shared<BundleAbilityFilterImpl>(bundleName);
}
}