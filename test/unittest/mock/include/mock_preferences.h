/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef APP_DOMAIN_VERIFY_MOCK_PREFERENCES_H
#define APP_DOMAIN_VERIFY_MOCK_PREFERENCES_H
#include <gmock/gmock.h>
#include "preferences.h"
#include <string>
namespace OHOS {
namespace NativePreferences {
void MockPreferences(std::shared_ptr<Preferences> pre, int errCode = 0);
void MockPreferencesRet(int ret);
class MocPreferences : public Preferences {
public:
    MocPreferences() = default;
    ~MocPreferences() override = default;
    MOCK_METHOD(std::string, GetString, (const std::string& key, const std::string& defValue), (override));
    MOCK_METHOD(int, PutString, (const std::string& key, const std::string& value), (override));
    MOCK_METHOD(int, RegisterObserver, (std::shared_ptr<PreferencesObserver> preferencesObserver, RegisterMode mode),
        (override));
    MOCK_METHOD(int, UnRegisterObserver, (std::shared_ptr<PreferencesObserver> preferencesObserver, RegisterMode mode),
        (override));

    PreferencesValue Get(const std::string& key, const PreferencesValue& defValue) override
    {
        return PreferencesValue();
    }
    int Put(const std::string& key, const PreferencesValue& value) override
    {
        return 0;
    }
    int GetInt(const std::string& key, const int& defValue = {}) override
    {
        return 0;
    }

    bool GetBool(const std::string& key, const bool& defValue = {}) override
    {
        return 0;
    }
    float GetFloat(const std::string& key, const float& defValue = {}) override
    {
        return 0;
    }
    double GetDouble(const std::string& key, const double& defValue = {}) override
    {

        return 0;
    }
    int64_t GetLong(const std::string& key, const int64_t& defValue = {}) override
    {
        return 0;
    }
    std::map<std::string, PreferencesValue> GetAll() override
    {
        return std::map<std::string, PreferencesValue>();
    }
    bool HasKey(const std::string& key) override
    {
        return 0;
    }
    int PutInt(const std::string& key, int value) override
    {
        return 0;
    }

    int PutBool(const std::string& key, bool value) override
    {
        return 0;
    }
    int PutLong(const std::string& key, int64_t value) override
    {
        return 0;
    }
    int PutFloat(const std::string& key, float value) override
    {
        return 0;
    }
    int PutDouble(const std::string& key, double value) override
    {
        return 0;
    }
    int Delete(const std::string& key) override
    {
        return 0;
    }
    int Clear() override
    {
        return 0;
    }
    void Flush() override
    {
    }
    int FlushSync() override
    {
        return 0;
    }
};
}
}
#endif  // APP_DOMAIN_VERIFY_MOCK_PREFERENCES_H
