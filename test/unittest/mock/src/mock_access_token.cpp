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
#include "mock_access_token.h"
#include "accesstoken_kit.h"
#include "access_token_error.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "tokenid_kit.h"
#include "gtest/gtest.h"

namespace OHOS::AppDomainVerify{
using namespace OHOS::Security::AccessToken;
static const std::string TEST_PERMISSION_GRANT = "ohos.permission.GET_APP_DOMAIN_BUNDLE_INFO";
int MockAccessToken::mockSystemApp()
{
    HapInfoParams infoParmsEnvironment = {
        .userID = 1,
        .bundleName = "InterfaceQueryPermission",
        .instIndex = 0,
        .dlpType = DLP_COMMON,
        .appIDDesc = "InterfaceQueryPermission",
        .isSystemApp = true
    };
    PermissionStateFull stateGrant = {
        .permissionName = TEST_PERMISSION_GRANT,
        .isGeneral = true,
        .resDeviceID = {"local"},
        .grantStatus = {PermissionState::PERMISSION_GRANTED},
        .grantFlags = {0}
    };
    HapPolicyParams policyParams = {
        .apl = APL_NORMAL,
        .domain = "test.domain",
        .permList = {},
        .permStateList = {stateGrant}
    };
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(infoParmsEnvironment, policyParams);
    EXPECT_NE(0, tokenIdEx.tokenIdExStruct.tokenID);
    EXPECT_EQ(true,  TokenIdKit::IsSystemAppByFullTokenID(tokenIdEx.tokenIDEx));
    EXPECT_EQ(0, SetSelfTokenID(tokenIdEx.tokenIDEx));
    return 0;
}
int MockAccessToken::mockSA()
{
    uint64_t tokenId;
    const char* perms[] = {};

    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = static_cast<int32_t>(sizeof(perms)/sizeof(perms[0])),
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_core",
    };
    infoInstance.processName = "memmgrservice";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    return 0;
}
}