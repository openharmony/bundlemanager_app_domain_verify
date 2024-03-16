/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "mock_bundle_manager.h"
#include <fstream>
#include <sstream>
#include <string>
#include "bundle_mgr_client.h"
#include "mock_constant.h"

using namespace OHOS::AAFwk;
namespace OHOS {

namespace AppExecFwk {
int BundleMgrStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "MOCK BundleMgrStub OnRemoteRequest";
    return 0;
}

ErrCode BundleMgrService::GetBundleInfoV9(const std::string &bundleName, int32_t flags, BundleInfo &bundleInfo,
    int32_t userId)
{
    GTEST_LOG_(INFO) << "MOCK BundleMgrService GetBundleInfoV9";
    bundleInfo.signatureInfo.fingerprint = AppDomainVerify::FINGERPRINT;
    return ERR_OK;
}

}  // namespace AppExecFwk
}  // namespace OHOS