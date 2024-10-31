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
#include "ability_filter.h"
#include "bundle_info_query.h"
#include "app_domain_verify_hilog.h"

namespace OHOS::AppDomainVerify {
using Skill = OHOS::AppExecFwk::Skill;
class AbilityFilterImpl : public AbilityFilter {
public:
    void SetBundleName(const std::string& bundleName) override
    {
        AbilityFilter::SetBundleName(bundleName);
    }
    bool Filter(const FilterInfo& info) override
    {
        bool result{ false };
        std::vector<AbilityInfo> abilityInfos;
        if (BundleInfoQuery::QueryAbilityInfosByUrl(info.bundleName, info.url, abilityInfos)) {
            result = !abilityInfos.empty();
        }
        return result;
    }
};

class BundleAbilityFilterImpl : public AbilityFilter {
public:
    void SetBundleName(const std::string& bundleName) override
    {
        this->bundleName_ = bundleName;
        Init();
    }
    bool Filter(const FilterInfo& info) override
    {
        Want want;
        want.SetUri(info.url);
        want.AddEntity(ENTITY_BROWSER);
        want.SetAction(ACTION_VIEW_DATA);

        auto matchedAbility = std::find_if(
            abilityInfos_.begin(), abilityInfos_.end(), [&want](const AbilityInfo& abilityInfo) {
                APP_DOMAIN_VERIFY_HILOGD(
                    APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "abilityInfo:%{private}s", abilityInfo.name.c_str());
                auto skills = abilityInfo.skills;
                auto matchedSkill = std::find_if(skills.begin(), skills.end(), [&want](const Skill& skill) {
                    APP_DOMAIN_VERIFY_HILOGD(
                        APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "domainVerify:%{private}d", skill.domainVerify);
                    if (skill.domainVerify && skill.Match(want)) {
                        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "skill matched");
                        return true;
                    }
                    return false;
                });
                if (matchedSkill != skills.end()) {
                    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "skill matched");
                    return true;
                }
                return true;
            });
        if (matchedAbility != abilityInfos_.end()) {
            APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "ability matched");
            return true;
        }
        return false;
    }
    void Init()
    {
        BundleInfoQuery::GetBundleInfosV9(bundleName_, abilityInfos_);
        APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MGR_MODULE_SERVICE, "abilityInfos size:%zu", abilityInfos_.size());
    }

private:
    std::vector<AbilityInfo> abilityInfos_;
    std::string bundleName_;
};
std::shared_ptr<AbilityFilter> AbilityFilter::Create()
{
    return std::make_shared<BundleAbilityFilterImpl>();
}
}