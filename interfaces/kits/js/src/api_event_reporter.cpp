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
#include "api_event_reporter.h"
#include <thread>
#include <mutex>
#include "app_event.h"
#include "app_event_processor_mgr.h"
#include "app_domain_verify_hilog.h"
#include "config_parser.h"

namespace OHOS::AppDomainVerify::Dfx {
#define API_REPORT_CONFIG_PATH "/system/etc/app_domain_verify/api_report.conf"
static int64_t g_processId = -1;
static std::mutex g_mutex;
constexpr int TRIGGER_COND_TIMEOUT = 90;
constexpr int TRIGGER_COND_ROW = 30;
static const std::string SDK_NAME("AbilityKit");

ApiEventReporter::ApiEventReporter(const std::string& apiName)
    : apiName_(apiName)
{
    transId_ = std::string("transId_") + std::to_string(std::rand());
    startTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch())
                     .count();
    std::unique_lock<std::mutex> lock(g_mutex);
    if (g_processId == -1) {
        g_processId = AddProcessor();
    }
}

int64_t ApiEventReporter::AddProcessor()
{
    HiviewDFX::HiAppEvent::ReportConfig config;
    config.appId = "app_domain_verify_ohos_sdk_ocg";
    config.name = "app_domain_verify_processor";
    ConfigParser parser;
    if (parser.load(API_REPORT_CONFIG_PATH)) {
        config.appId = parser.get("report_appId");
        config.name = parser.get("report_name");
    }
    config.routeInfo = "AUTO";
    config.triggerCond.timeout = TRIGGER_COND_TIMEOUT;
    config.triggerCond.row = TRIGGER_COND_ROW;

    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_JS_NAPI,
        "AddProcessor appId:%{public}s, name:%{public}s, routeInfo:%{public}s", config.appId.c_str(),
        config.name.c_str(), config.routeInfo.c_str());

    config.eventConfigs.clear();
    {
        HiviewDFX::HiAppEvent::EventConfig event1;
        event1.domain = "api_diagnostic";
        event1.name = "api_exec_end";
        event1.isRealTime = false;
        config.eventConfigs.push_back(event1);
    }

    {
        HiviewDFX::HiAppEvent::EventConfig event2;
        event2.domain = "api_diagnostic";
        event2.name = "api_called_stat";
        event2.isRealTime = true;
        config.eventConfigs.push_back(event2);
    }

    {
        HiviewDFX::HiAppEvent::EventConfig event3;
        event3.domain = "api_diagnostic";
        event3.name = "api_called_stat_cnt";
        event3.isRealTime = true;
        config.eventConfigs.push_back(event3);
    }
    return HiviewDFX::HiAppEvent::AppEventProcessorMgr::AddProcessor(config);
}

void ApiEventReporter::WriteEndEvent(const int result, const int32_t errCode)
{
    int64_t endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch())
                          .count();
    HiviewDFX::HiAppEvent::Event event("api_diagnostic", "api_exec_end", HiviewDFX::HiAppEvent::BEHAVIOR);
    event.AddParam("trans_id", transId_);
    event.AddParam("api_name", apiName_);
    event.AddParam("sdk_name", SDK_NAME);
    event.AddParam("begin_time", startTime_);
    event.AddParam("end_time", endTime);
    event.AddParam("result", result);
    event.AddParam("error_code", errCode);
    int ret = Write(event);
    APP_DOMAIN_VERIFY_HILOGD(APP_DOMAIN_VERIFY_MODULE_JS_NAPI,
        "WriteEndEvent transId:%{public}s, apiName:%{public}s, sdkName:%{public}s, result:%{public}d, errCode:%{public}d, ret:%{public}d",
        transId_.c_str(), apiName_.c_str(), SDK_NAME.c_str(), result, errCode, ret);
}
}