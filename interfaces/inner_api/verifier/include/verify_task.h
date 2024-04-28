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

#ifndef APP_DOMAIN_VERIFY_VERIFY_TASK_H
#define APP_DOMAIN_VERIFY_VERIFY_TASK_H
#include "i_verify_task.h"
#include "http_client_response.h"
#include "app_verify_base_info.h"
#include "inner_verify_status.h"
#include "app_domain_verify_hisysevent.h"
#include "skill_uri.h"
#include "app_domain_verify_mgr_client.h"

namespace OHOS {
namespace AppDomainVerify {
class VerifyTask : public IVerifyTask {
public:
    void OnPostVerify(const std::string &uri, const OHOS::NetStack::HttpClient::HttpClientResponse &response) override;
    void OnSaveVerifyResult() override;
    bool OnPreRequest(OHOS::NetStack::HttpClient::HttpClientRequest &request, const std::string &uri) override;
    OHOS::AppDomainVerify::TaskType GetType() override;
    const std::unordered_map<std::string, InnerVerifyStatus> &GetUriVerifyMap() override;

    VerifyTask(OHOS::AppDomainVerify::TaskType type, const AppVerifyBaseInfo &appVerifyBaseInfo,
        const std::vector<SkillUri> &skillUris);
    void InitUriVerifyMap(const std::vector<SkillUri> &skillUris);

protected:
    OHOS::AppDomainVerify::TaskType& GetTaskType();
    AppVerifyBaseInfo& GetAppVerifyBaseInfo();
    std::unordered_map<std::string, InnerVerifyStatus>& GetInnerUriVerifyMap();
private:
    virtual bool SaveDomainVerifyStatus(const std::string& bundleName, const VerifyResultInfo& verifyResultInfo);
    OHOS::AppDomainVerify::TaskType type_;
    AppVerifyBaseInfo appVerifyBaseInfo_;
    std::unordered_map<std::string, InnerVerifyStatus> uriVerifyMap_;
};
}
}
#endif  // APP_DOMAIN_VERIFY_VERIFY_TASK_H
