/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef APP_DOMAIN_VERIFY_APP_DETAILS_FILTER_H
#define APP_DOMAIN_VERIFY_APP_DETAILS_FILTER_H

#include "filter_define.h"
#include "want.h"
#include <string>

namespace OHOS {
namespace AppDomainVerify {
    class AppDetailsFilter {
    public:
        bool Filter(const std::vector<AppDetailInfo>& input, std::vector<AppDetailInfo>& dest, const std::string& url);
    };
}
}
#endif