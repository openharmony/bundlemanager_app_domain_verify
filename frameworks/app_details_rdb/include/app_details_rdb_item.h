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

#ifndef APP_DOMAIN_VERIFY_SERVICE_APP_DETAILS_RDB_ITEM_H
#define APP_DOMAIN_VERIFY_SERVICE_APP_DETAILS_RDB_ITEM_H
#include <string>
#include "app_details_base_item.h"
namespace OHOS {
namespace AppDomainVerify {
class AppDetailsRdbItem :public AppDetailsBaseItem {
public:
    bool GetRdbItem(std::shared_ptr<AbsSharedResultSet>& retSet) override;
    void AddRdbItemToBucket(NativeRdb::ValuesBucket& bucket) override;
public:
    std::string id;
    std::string scheme;
    std::string domain;
    std::string pathType;
    std::string path;
    std::string bundleName;
};
}
}


#endif