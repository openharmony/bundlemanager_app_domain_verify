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
#include "setting_data_share_helper.h"
#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "iservice_registry.h"
#include "uri.h"
#include "singleton.h"
#include "app_domain_verify_hilog.h"

namespace OHOS::AppDomainVerify {
const std::string SettingsDataShareHelper::SETTINGS_DATASHARE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
const std::string SETTINGS_DATASHARE_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
constexpr const char* SETTINGS_DATA_COLUMN_KEYWORD = "KEYWORD";
constexpr const char* SETTINGS_DATA_COLUMN_VALUE = "VALUE";

SettingsDataShareHelper::SettingsDataShareHelper() = default;

SettingsDataShareHelper::~SettingsDataShareHelper() = default;

std::shared_ptr<DataShare::DataShareHelper> SettingsDataShareHelper::CreateDataShareHelper(int systemAbilityId)
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "GetSystemAbilityManager failed.");
        return nullptr;
    }
    sptr<IRemoteObject> remote = saManager->GetSystemAbility(systemAbilityId);
    if (remote == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "GetSystemAbility Service Failed.");
        return nullptr;
    }
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "systemAbilityId = %{public}d", systemAbilityId);
    return DataShare::DataShareHelper::Creator(remote, SETTINGS_DATASHARE_URI, SETTINGS_DATASHARE_EXT_URI);
}

int32_t SettingsDataShareHelper::Query(Uri& uri, const std::string& key, std::string& value)
{
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "start Query");
    std::shared_ptr<DataShare::DataShareHelper> settingHelper = CreateDataShareHelper(
        APP_DOMAIN_VERIFY_AGENT_SA_ID);
    if (settingHelper == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "query error, datashareHelper_ is nullptr");
        return -1;
    }

    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    auto result = settingHelper->Query(uri, predicates, columns);
    if (result == nullptr) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "query error, result is nullptr");
        settingHelper->Release();
        return -1;
    }

    int rowCount = 0;
    result->GetRowCount(rowCount);
    if (rowCount == 0) {
        APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "query success, but rowCount is 0");
        settingHelper->Release();
        return 0;
    }

    if (result->GoToFirstRow() != DataShare::E_OK) {
        APP_DOMAIN_VERIFY_HILOGE(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "query error, go to first row error");
        result->Close();
        settingHelper->Release();
        return -1;
    }

    int columnIndex = 0;
    result->GetColumnIndex(SETTINGS_DATA_COLUMN_VALUE, columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    settingHelper->Release();
    APP_DOMAIN_VERIFY_HILOGI(APP_DOMAIN_VERIFY_AGENT_MODULE_SERVICE, "SettingUtils: query success");
    return 0;
}
}