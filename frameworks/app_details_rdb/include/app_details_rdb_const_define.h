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

#ifndef APP_DOMAIN_VERIFY_SERVICE_APP_DETAILS_CONST_DEFINE_H
#define APP_DOMAIN_VERIFY_SERVICE_APP_DETAILS_CONST_DEFINE_H
#include <stdint.h>
#include <string>

namespace OHOS {
namespace AppDomainVerify {

const std::string DETAILS_ID = "ID";
const std::string DETAILS_SCHEME = "SCHEME";
const std::string DETAILS_DOMAIN = "DOMAIN";
const std::string DETAILS_PATH_TYPE = "PATH_TYPE";
const std::string DETAILS_PATH = "PATH";
const std::string DETAILS_BUNDLE_NAME = "BUNDLE_NAME";
const int32_t DETAILS_ID_INDEX = 0;
const int32_t DETAILS_SCHEME_INDEX = 1;
const int32_t DETAILS_DOMAIN_INDEX = 2;
const int32_t DETAILS_PATH_TYPE_INDEX = 3;
const int32_t DETAILS_PATH_INDEX = 4;
const int32_t DETAILS_BUNDLE_NAME_INDEX = 5;

const std::string META_TABLE_NAME = "TABLE_NAME";
const std::string META_TABLE_VERSION = "TABLE_VERSION";
const std::string META_TABLE_EXT_INFO = "TABLE_EXT_INFO";
const std::string META_UPDATE_TIME = "UPDATE_TIME";
const int32_t META_TABLE_NAME_INDEX = 0;
const int32_t META_TABLE_VERSION_INDEX = 1;
const int32_t META_TABLE_EXT_INFO_INDEX = 2;
const int32_t META_UPDATE_TIME_INDEX = 3;

constexpr int32_t CLOSE_DELAY_TIME_S = 20000;  // 20s
const std::string CLOSE_TASK_ID = "closeRdb";
const std::string APP_DETAILS_TABLE = "APP_DETAILS";
const std::string META_DATA = "META_DATA";
const std::string APP_DETAILS_RDB_PATH = "/data/service/el1/public/app_domain_verify_agent_service/";
const std::string APP_DETAILS_RDB_NAME = "appDetails.db";
const int APP_DETAILS_RDB_VERSION = 1;

}
}

#endif