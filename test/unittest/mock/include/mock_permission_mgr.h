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

#ifndef APP_DOMAIN_VERIFY_MOCK_PERMISSION_MGR_H
#define APP_DOMAIN_VERIFY_MOCK_PERMISSION_MGR_H
#include <memory>
#include <gmock/gmock.h>
#include "permission_manager.h"
namespace OHOS::AppDomainVerify {
class MocIPermissionManager {
public:
    MocIPermissionManager() = default;
    virtual ~MocIPermissionManager() = default;
    virtual bool CheckPermission(const std::string& permission) = 0;
    virtual bool IsSystemAppCall() = 0;
    virtual bool IsSACall() = 0;
    virtual bool IsAgentCall() = 0;
};
class MocPermissionManager : public MocIPermissionManager {
public:
    MOCK_METHOD(bool, CheckPermission, (const std::string& permission), (override));
    MOCK_METHOD(bool, IsSystemAppCall, (), (override));
    MOCK_METHOD(bool, IsSACall, (), (override));
    MOCK_METHOD(bool, IsAgentCall, (), (override));
};
void DoMocPermissionManager(std::shared_ptr<MocIPermissionManager> moc);
}
#endif  // APP_DOMAIN_VERIFY_MOCK_PERMISSION_MGR_H
