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
#include "gtest/gtest.h"
#include "mock_constant.h"
#define private public
#define protected public
#include "ability_filter.h"
#undef private
#undef protected
#include "bms/bundle_info_query.h"
#include "mock_bundle_manager.h"
#include "mock_system_ability.h"
#include "mock_system_ability_registry.h"
#include "mock_os_accout_manager.h"

namespace OHOS::AppDomainVerify {
using Want = OHOS::AAFwk::Want;
using namespace OHOS::AppExecFwk;
using namespace testing;
using namespace testing::ext;
constexpr const char* BUNDLE_NAME_WRONG = "BUNDLE_NAME_WRONG";
constexpr const char* BUNDLE_DOMAIN = "https://www.openharmony.cn";
constexpr const char* BUNDLE_DOMAIN_WRONG = "https://www.openharmony_wrong.cn";
constexpr const char* BUNDLE_URL = "https://www.openharmony.cn/100";
constexpr const char* BUNDLE_URL_NEW = "https://www.openharmony.cn/new";
constexpr const char* TASK_ID = "age";
constexpr int MAX_CACHE_SIZE = 50;
constexpr int FLAGS = AppExecFwk::BundleFlag::GET_BUNDLE_WITH_ABILITIES | AppExecFwk::BundleFlag::GET_BUNDLE_WITH_SKILL;
class AbilityFilterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};
std::vector<AbilityInfo> g_mockAbilityInfos;
void AbilityFilterTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase";
    OHOS::AccountSA::g_enableMockIds = true;
    g_mockBundleMgrService = new AppExecFwk::BundleMgrService();
}

void AbilityFilterTest::TearDownTestCase(void)
{
    g_mockAbilityInfos.clear();
}

void AbilityFilterTest::SetUp(void)
{
}

void AbilityFilterTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "TearDown";
    g_mockBundleMgrService->impl = nullptr;
    OHOS::AccountSA::g_accountIds.clear();
}

bool InvokeGetBundleInfo(const std::string& bundleName, int flag, BundleInfo& bundleInfo, int32_t userId)
{
    bundleInfo.abilityInfos = g_mockAbilityInfos;
    return true;
}
/**
 * @tc.name: AbilityFilterTest001
 * @tc.desc: filter success.
 * @tc.type: FUNC
 */
HWTEST_F(AbilityFilterTest, AbilityFilterTest001, TestSize.Level0)
{
    std::vector<std::string> actions = { ACTION_VIEW_DATA };
    std::vector<std::string> entities = { ENTITY_BROWSER };
    std::vector<SkillUri> uris = { { .scheme = "https", .host = "www.openharmony.cn" } };
    Skill skill;
    skill.domainVerify = true;
    skill.actions = actions;
    skill.entities = entities;
    skill.uris = uris;
    std::vector<Skill> skills = { skill };
    AbilityInfo abilityInfo;
    abilityInfo.skills = skills;
    g_mockAbilityInfos.push_back(abilityInfo);
    auto mocBundleMgrService = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService, GetBundleInfo(_, FLAGS, _, _)).WillOnce(Invoke(InvokeGetBundleInfo));
    g_mockBundleMgrService->impl = mocBundleMgrService;

    auto filter = AbilityFilter::Create(BUNDLE_NAME);
    EXPECT_TRUE(filter->Filter({ .url = BUNDLE_URL }));
}

/**
 * @tc.name: AbilityFilterTest002
 * @tc.desc: filter with bms error.
 * @tc.type: FUNC
 */
HWTEST_F(AbilityFilterTest, AbilityFilterTest002, TestSize.Level0)
{
    auto mocBundleMgrService = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService, GetBundleInfo(_, FLAGS, _, _)).WillOnce(Return(false));
    g_mockBundleMgrService->impl = mocBundleMgrService;

    auto filter = AbilityFilter::Create(BUNDLE_NAME);
    EXPECT_FALSE(filter->Filter({ .url = BUNDLE_URL }));
}

/**
 * @tc.name: AbilityFilterTest003
 * @tc.desc: filter with no abilities.
 * @tc.type: FUNC
 */
HWTEST_F(AbilityFilterTest, AbilityFilterTest003, TestSize.Level0)
{
    auto mocBundleMgrService = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService, GetBundleInfo(_, FLAGS, _, _)).WillOnce(Return(true));
    g_mockBundleMgrService->impl = mocBundleMgrService;

    auto filter = AbilityFilter::Create(BUNDLE_NAME);
    EXPECT_FALSE(filter->Filter({ .url = BUNDLE_URL }));
}

/**
 * @tc.name: AbilityFilterTest004
 * @tc.desc: filter with no skill.
 * @tc.type: FUNC
 */
HWTEST_F(AbilityFilterTest, AbilityFilterTest004, TestSize.Level0)
{
    std::vector<Skill> skills;
    AbilityInfo abilityInfo;
    abilityInfo.skills = skills;
    g_mockAbilityInfos.push_back(abilityInfo);
    auto mocBundleMgrService = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService, GetBundleInfo(_, FLAGS, _, _)).WillOnce(Return(true));
    g_mockBundleMgrService->impl = mocBundleMgrService;

    auto filter = AbilityFilter::Create(BUNDLE_NAME);
    EXPECT_FALSE(filter->Filter({ .url = BUNDLE_URL }));
}

/**
 * @tc.name: AbilityFilterTest005
 * @tc.desc: filter with no domainVerify skill.
 * @tc.type: FUNC
 */
HWTEST_F(AbilityFilterTest, AbilityFilterTest005, TestSize.Level0)
{
    std::vector<std::string> actions = { ACTION_VIEW_DATA };
    std::vector<std::string> entities = { ENTITY_BROWSER };
    std::vector<SkillUri> uris = { { .scheme = "https", .host = "www.openharmony.cn" } };
    Skill skill;
    skill.domainVerify = false;
    skill.actions = actions;
    skill.entities = entities;
    skill.uris = uris;
    std::vector<Skill> skills = { skill };
    AbilityInfo abilityInfo;
    abilityInfo.skills = skills;
    g_mockAbilityInfos.push_back(abilityInfo);
    auto mocBundleMgrService = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService, GetBundleInfo(_, FLAGS, _, _)).WillOnce(Return(true));
    g_mockBundleMgrService->impl = mocBundleMgrService;

    auto filter = AbilityFilter::Create(BUNDLE_NAME);
    EXPECT_FALSE(filter->Filter({ .url = BUNDLE_URL }));
}

/**
 * @tc.name: AbilityFilterTest006
 * @tc.desc: filter with no matched skill.
 * @tc.type: FUNC
 */
HWTEST_F(AbilityFilterTest, AbilityFilterTest006, TestSize.Level0)
{
    std::vector<std::string> actions = { ACTION_VIEW_DATA };
    std::vector<std::string> entities = { ENTITY_BROWSER };
    std::vector<SkillUri> uris = { { .scheme = "https", .host = "www.openharmony1.cn" } };
    Skill skill;
    skill.domainVerify = true;
    skill.actions = actions;
    skill.entities = entities;
    skill.uris = uris;
    std::vector<Skill> skills = { skill };
    AbilityInfo abilityInfo;
    abilityInfo.skills = skills;
    g_mockAbilityInfos.push_back(abilityInfo);
    auto mocBundleMgrService = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService, GetBundleInfo(_, FLAGS, _, _)).WillOnce(Return(true));
    g_mockBundleMgrService->impl = mocBundleMgrService;

    auto filter = AbilityFilter::Create(BUNDLE_NAME);
    EXPECT_FALSE(filter->Filter({ .url = BUNDLE_URL }));
}
}