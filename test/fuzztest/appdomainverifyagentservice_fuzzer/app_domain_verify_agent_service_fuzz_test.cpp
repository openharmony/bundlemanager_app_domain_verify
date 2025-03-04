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

#include "app_domain_verify_agent_service_fuzz_test.h"

#include <cstddef>
#include <cstdint>

#include "app_domain_verify_agent_service.h"

using namespace OHOS::AppExecFwk;
namespace OHOS {
constexpr size_t U32_AT_SIZE = 4;
constexpr uint32_t CODE_MAX = 3;
static AppDomainVerify::AppDomainVerifyAgentService g_appDomainVerifyAgentService;
const std::u16string INTERFACE_TOKEN = u"ohos.appDomainVerify.IAppDomainVerifyAgentService";
;

bool DoSomethingInterestingWithMyAPI(const uint8_t* rawData, size_t size)
{
    for (uint32_t code = 0; code <= CODE_MAX; code++) {
        MessageParcel data;
        data.WriteInterfaceToken(INTERFACE_TOKEN);
        data.WriteBuffer(rawData, size);
        data.RewindRead(0);
        MessageParcel reply;
        MessageOption option;
        g_appDomainVerifyAgentService.OnRemoteRequest(code, data, reply, option);
    }
    return true;
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    if (size < OHOS::U32_AT_SIZE) {
        return 0;
    }

    char* ch = static_cast<char*>(malloc(size + 1));
    if (ch == nullptr) {
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size, data, size) != EOK) {
        free(ch);
        ch = nullptr;
        return 0;
    }

    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    free(ch);
    ch = nullptr;
    return 0;
}
}