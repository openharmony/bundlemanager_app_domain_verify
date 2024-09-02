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

#include "common_utils.h"
namespace OHOS::AppDomainVerify {
constexpr int MAX_STEP = 3;
constexpr size_t MAX_MASK_LEN = 50;
std::string MaskStr(const std::string& input)
{
    if (input.empty()) {
        return "";
    }
    auto len = input.length();
    len = std::min(len, MAX_MASK_LEN);
    auto ret = input;
    for (auto i = 0; i < len; i++) {
        if (i % MAX_STEP == 0) {
            ret[i] = input[i];
        } else {
            ret[i] = '*';
        }
    }
    return ret;
}
}