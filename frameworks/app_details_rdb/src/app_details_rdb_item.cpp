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

#include "app_details_rdb_item.h"
#include "app_details_rdb_const_define.h"
#include "rdb_errno.h"
namespace OHOS {
namespace AppDomainVerify {

bool AppDetailsRdbItem::GetRdbItem(std::shared_ptr<AbsSharedResultSet> &retSet)
{
    int errCode = NativeRdb::E_OK;
    errCode = retSet->GetString(DETAILS_ID_INDEX, id);
    if (errCode != NativeRdb::E_OK) {
        return false;
    }
    errCode = retSet->GetString(DETAILS_SCHEME_INDEX, scheme);
    if (errCode != NativeRdb::E_OK) {
        return false;
    }
    errCode = retSet->GetString(DETAILS_DOMAIN_INDEX, domain);
    if (errCode != NativeRdb::E_OK) {
        return false;
    }
    errCode = retSet->GetString(DETAILS_PATH_TYPE_INDEX, pathType);
    if (errCode != NativeRdb::E_OK) {
        return false;
    }
    errCode = retSet->GetString(DETAILS_PATH_INDEX, path);
    if (errCode != NativeRdb::E_OK) {
        return false;
    }
    errCode = retSet->GetString(DETAILS_BUNDLE_NAME_INDEX, bundleName);
    if (errCode != NativeRdb::E_OK) {
        return false;
    }
    return true;
};

void AppDetailsRdbItem::AddRdbItemToBucket(NativeRdb::ValuesBucket &bucket)
{
    bucket.PutString(DETAILS_ID, id);
    bucket.PutString(DETAILS_SCHEME, scheme);
    bucket.PutString(DETAILS_DOMAIN, domain);
    bucket.PutString(DETAILS_PATH_TYPE, pathType);
    bucket.PutString(DETAILS_PATH, path);
    bucket.PutString(DETAILS_BUNDLE_NAME, bundleName);
}


}
}