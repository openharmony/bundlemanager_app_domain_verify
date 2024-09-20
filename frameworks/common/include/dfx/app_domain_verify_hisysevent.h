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
#ifndef APP_DOMAIN_VERIFY_HISYSTEM_EVENT_H
#define APP_DOMAIN_VERIFY_HISYSTEM_EVENT_H
#include <string>
#include "hisysevent.h"

namespace OHOS {
namespace AppDomainVerify {
static constexpr char APP_DOMAIN_VERIFY[] = "APPDOMAINVERIFY";
namespace EventType {
const std::string APP_INSTALL_EVENT = "PKG_INSTALL";
const std::string APP_DELETE_EVENT = "PKG_UNINSTALL";
const std::string APP_VERIFY_EVENT = "PKG_VERIFY_RESULT";
const std::string APP_UNIVERSAL_ERROR_EVENT = "UNIVERSAL_ERROR";
}
namespace EventParamKey {
const std::string APP_ID = "APP_ID";
const std::string BUNDLE_NAME = "BUNDLE_NAME";
const std::string VERIFY_STATUS = "VERIFY_STATUS";
const std::string VERIFY_WAY = "VERIFY_WAY";
const std::string ERROR_CODE = "ERROR_CODE";
}
typedef enum EnumTaskType {
    IMMEDIATE_TASK,
    BOOT_REFRESH_TASK,
    SCHEDULE_REFRESH_TASK,
    UNKNOWN_TASK
} TaskType;
enum EventCode : uint32_t {
    COMMON_FAULT_START = 0x00000,
    SIGN_FAULT = 0x00001,
    GET_GATEWAY_FAULT = 0x00002,
    CONNECT_MGR_FAULT = 0x00003,
    CONNECT_AGENT_FAULT = 0x00004,
    CONNECT_OTHER_FAULT = 0x00005,
    GET_EXT_FAULT = 0x00006,
    UPDATE_DB_FAULT = 0x00007,
    COMMON_FAULT_END,

    BASE_LINK_FAULT_START = 0x10000,
    LOAD_DB_FAULT = 0x10001,
    GET_DATE_IN_BOOT_FAULT = 0x12001,
    GET_DATE_IN_LOOP_FAULT = 0x12101,
    DEL_DB_IN_WRITE_BACK_FAULT = 0x13001,
    WRITE_DB_IN_WRITE_BACK_FAULT = 0x13002,
    BASE_LINK_FAULT_END,

    SHORT_LINK_FAULT_START = 0x20000,
    SYNC_WHITE_LIST_FAULT = 0x20001,
    WRITE_DYNAMIC_WHITE_LIST_FAULT = 0x20002,
    READ_DEFAULT_WHITE_LIST_FAULT = 0x20101,
    READ_DYNAMIC_WHITE_LIST_FAULT = 0x20102,
    READ_CACHE_FAULT = 0x21001,
    NET_UNREACHED_FAULT = 0x21101,
    CONVERT_TO_WANT_FAULT = 0x21102,
    CALL_BACK_FAULT = 0x21201,
    UPDATE_CACHE_FAULT = 0x21301,
    SHORT_LINK_FAULT_END
};

#define INSTALL_EVENT(appIdentifier, bundleName)                                                      \
    do {                                                                                              \
        HiSysEventWrite(APP_DOMAIN_VERIFY, EventType::APP_INSTALL_EVENT,                              \
            OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, EventParamKey::APP_ID, (appIdentifier), \
            EventParamKey::BUNDLE_NAME, (bundleName));                                                \
    } while (0)

#define UNINSTALL_EVENT(appIdentifier, bundleName)                                                    \
    do {                                                                                              \
        HiSysEventWrite(APP_DOMAIN_VERIFY, EventType::APP_DELETE_EVENT,                               \
            OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, EventParamKey::APP_ID, (appIdentifier), \
            EventParamKey::BUNDLE_NAME, (bundleName));                                                \
    } while (0)

#define VERIFY_RESULT_EVENT(appIdentifier, bundleName, type, status)                                                   \
    do {                                                                                                               \
        HiSysEventWrite(APP_DOMAIN_VERIFY, EventType::APP_VERIFY_EVENT,                                                \
            OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, EventParamKey::APP_ID, (appIdentifier),                  \
            EventParamKey::BUNDLE_NAME, (bundleName), EventParamKey::VERIFY_WAY, (type), EventParamKey::VERIFY_STATUS, \
            (status));                                                                                                 \
    } while (0)

#define UNIVERSAL_ERROR_EVENT(errorCode)                                                            \
    do {                                                                                            \
        HiSysEventWrite(APP_DOMAIN_VERIFY, EventType::APP_UNIVERSAL_ERROR_EVENT,                    \
            OHOS::HiviewDFX::HiSysEvent::EventType::FAULT, EventParamKey::ERROR_CODE, (errorCode)); \
    } while (0)

}
}
#endif