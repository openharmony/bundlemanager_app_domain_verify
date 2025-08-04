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

#include "gtest/gtest.h"
#include <memory>
#include "mock_constant.h"
#define private public
#define protected public
#include "bms/bundle_info_query.h"
#undef private
#undef protected
#include "mock_bundle_manager.h"
#include "mock_system_ability.h"
#include "mock_system_ability_registry.h"
#include "mock_os_accout_manager.h"

namespace OHOS::AppDomainVerify {
using namespace testing;
using namespace testing::ext;

class BundleInfoQueryTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void BundleInfoQueryTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase";
    OHOS::AccountSA::g_enableMockIds = true;
    g_mockBundleMgrService = new AppExecFwk::BundleMgrService();
}

void BundleInfoQueryTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase";
}

void BundleInfoQueryTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "SetUp";
}

void BundleInfoQueryTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "TearDown";
    g_mockBundleMgrService->impl = nullptr;
    OHOS::AccountSA::g_accountIds.clear();
}

/**
 * @tc.name: BundleInfoQueryTest001
 * @tc.desc: GetBundleInfo test.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, BundleInfoQueryTest001, TestSize.Level0)
{
    OHOS::AccountSA::g_accountIds.push_back(1);
    std::string bundleName = BUNDLE_NAME;
    std::string appIdentifier = APP_IDENTIFIER;
    std::string fingerprint = FINGERPRINT;
    ASSERT_TRUE(BundleInfoQuery::GetBundleInfo(bundleName, appIdentifier, fingerprint));
}
/**
 * @tc.name: BundleInfoQueryTest002
 * @tc.desc: GetBundleInfo no system ability.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, BundleInfoQueryTest002, TestSize.Level0)
{
    OHOS::g_disableSystemAbilityManager = true;
    std::string bundleName = BUNDLE_NAME;
    std::string appIdentifier = APP_IDENTIFIER;
    std::string fingerprint = FINGERPRINT;
    ASSERT_FALSE(BundleInfoQuery::GetBundleInfo(bundleName, appIdentifier, fingerprint));
    OHOS::g_disableSystemAbilityManager = false;
}
/**
 * @tc.name: BundleInfoQueryTest003
 * @tc.desc: GetBundleInfo failed.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, BundleInfoQueryTest003, TestSize.Level0)
{
    auto mocBundleMgrService = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService, GetBundleInfoV9(_, _, _, _)).WillOnce(Return(ERR_NO_MEMORY));
    g_mockBundleMgrService->impl = mocBundleMgrService;
    OHOS::AccountSA::g_accountIds.push_back(1);
    printf("BundleInfoQueryTest003 g_mockBundleMgrService:%p\n", g_mockBundleMgrService.GetRefPtr());
    std::string bundleName = BUNDLE_NAME;
    std::string appIdentifier = APP_IDENTIFIER;
    std::string fingerprint = FINGERPRINT;
    ASSERT_FALSE(BundleInfoQuery::GetBundleInfo(bundleName, appIdentifier, fingerprint));
}
/**
 * @tc.name: BundleInfoQueryTest004
 * @tc.desc: GetBundleInfo get accoutInfo empty.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, BundleInfoQueryTest004, TestSize.Level0)
{
    std::string bundleName = BUNDLE_NAME;
    std::string appIdentifier = APP_IDENTIFIER;
    std::string fingerprint = FINGERPRINT;
    ASSERT_FALSE(BundleInfoQuery::GetBundleInfo(bundleName, appIdentifier, fingerprint));
}
/**
 * @tc.name: BundleInfoQueryTest005
 * @tc.desc: GetBundleInfo get accoutInfo -1.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, BundleInfoQueryTest005, TestSize.Level0)
{
    OHOS::AccountSA::g_accountIds.push_back(-1);
    std::string bundleName = BUNDLE_NAME;
    std::string appIdentifier = APP_IDENTIFIER;
    std::string fingerprint = FINGERPRINT;
    ASSERT_FALSE(BundleInfoQuery::GetBundleInfo(bundleName, appIdentifier, fingerprint));
    OHOS::AccountSA::g_accountIds.clear();
}
/**
 * @tc.name: BundleInfoQueryTest006
 * @tc.desc: GetBundleInfo get accoutInfo all -1.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, BundleInfoQueryTest006, TestSize.Level0)
{
    OHOS::AccountSA::g_accountIds.push_back(-1);
    OHOS::AccountSA::g_accountIds.push_back(-2);
    std::string bundleName = BUNDLE_NAME;
    std::string appIdentifier = APP_IDENTIFIER;
    std::string fingerprint = FINGERPRINT;
    ASSERT_FALSE(BundleInfoQuery::GetBundleInfo(bundleName, appIdentifier, fingerprint));
}
/**
 * @tc.name: BundleInfoQueryTest007
 * @tc.desc: GetBundleInfo get accoutInfo failed.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, BundleInfoQueryTest007, TestSize.Level0)
{
    OHOS::AccountSA::g_accountIds.push_back(-1);
    OHOS::AccountSA::g_accountIds.push_back(-2);
    OHOS::AccountSA::g_accountIds.push_back(0);
    std::string bundleName = BUNDLE_NAME;
    std::string appIdentifier = APP_IDENTIFIER;
    std::string fingerprint = FINGERPRINT;
    ASSERT_TRUE(BundleInfoQuery::GetBundleInfo(bundleName, appIdentifier, fingerprint));
}
/**
 * @tc.name: BundleInfoQueryTest008
 * @tc.desc: GetBundleInfo get accoutInfo failed.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, BundleInfoQueryTest008, TestSize.Level0)
{
    OHOS::AccountSA::g_queryRet = -1;
    OHOS::AccountSA::g_accountIds.push_back(-1);
    OHOS::AccountSA::g_accountIds.push_back(-2);
    OHOS::AccountSA::g_accountIds.push_back(0);
    std::string bundleName = BUNDLE_NAME;
    std::string appIdentifier = APP_IDENTIFIER;
    std::string fingerprint = FINGERPRINT;
    ASSERT_FALSE(BundleInfoQuery::GetBundleInfo(bundleName, appIdentifier, fingerprint));
}

/**
 * @tc.name: BundleGetBundleNameForUidTest001
 * @tc.desc: GetBundleName success.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, BundleGetBundleNameForUidTest001, TestSize.Level0)
{
    auto mocBundleMgrService = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService, GetBundleNameForUid(_, _)).WillOnce(Return(true));
    g_mockBundleMgrService->impl = mocBundleMgrService;
    int id = 0;
    std::string bundleName;
    ASSERT_TRUE(BundleInfoQuery::GetBundleNameForUid(id, bundleName));
}
/**
 * @tc.name: QueryAbilityInfosTest001
 * @tc.desc: QueryAbilityInfos success.
 * @tc.type: FUNC
 */
HWTEST_F(BundleInfoQueryTest, QueryAbilityInfosTest001, TestSize.Level0)
{
    auto mocBundleMgrService = std::make_shared<OHOS::AppExecFwk::MocBundleMgrService>();
    EXPECT_CALL(*mocBundleMgrService,
        ImplicitQueryInfos(_, _, _, _, _, _, _))
        .WillOnce(Return(true));
    g_mockBundleMgrService->impl = mocBundleMgrService;
    std::string url{ "openharmony.com" };
    std::vector<OHOS::AppExecFwk::AbilityInfo> originAbilityInfos;
    bool findDefault{ false };
    ASSERT_TRUE(BundleInfoQuery::QueryAbilityInfos(url, true, originAbilityInfos, findDefault));
}
}