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

#ifndef APP_DOMAIN_VERIFY_SERVICE_INCLUDE_SCOPE_GUARD_H
#define APP_DOMAIN_VERIFY_SERVICE_INCLUDE_SCOPE_GUARD_H

#include <string>

namespace OHOS {
namespace AppDomainVerify {
class ScopeGuard final {
public:
    using Function = std::function<void()>;
    explicit ScopeGuard(Function fn)
        : fn_(fn),
          dismissed_(false)
    {
    }

    ~ScopeGuard()
    {
        if (!dismissed_) {
            fn_();
        }
    }

    void Dismiss()
    {
        dismissed_ = true;
    }

private:
    Function fn_;
    bool dismissed_;
};
}
}
#endif