/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "app_details_meta_item.h"
#include "app_details_rdb_const_define.h"

namespace OHOS {
namespace AppDomainVerify {

bool MetaItem::GetRdbItem(std::shared_ptr<AbsSharedResultSet> &retSet)
{
    int errCode = NativeRdb::E_OK;
    errCode = retSet->GetString(META_TABLE_NAME_INDEX, tableName);
    if (errCode != NativeRdb::E_OK) {
        return false;
    }
    errCode = retSet->GetString(META_TABLE_VERSION_INDEX, tableVersion);
    if (errCode != NativeRdb::E_OK) {
        return false;
    }
    errCode = retSet->GetString(META_TABLE_EXT_INFO_INDEX, tableExtInfo);
    if (errCode != NativeRdb::E_OK) {
        return false;
    }
    errCode = retSet->GetString(META_UPDATE_TIME_INDEX, updateTime);
    if (errCode != NativeRdb::E_OK) {
        return false;
    }
    return true;
};

void MetaItem::AddRdbItemToBucket(NativeRdb::ValuesBucket &bucket)
{
    bucket.PutString(META_TABLE_NAME, tableName);
    bucket.PutString(META_TABLE_VERSION, tableVersion);
    bucket.PutString(META_TABLE_EXT_INFO, tableExtInfo);
    bucket.PutString(META_UPDATE_TIME, updateTime);
};
}
}
