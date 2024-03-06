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
#include "verify_http_task_factory.h"
namespace OHOS {
namespace AppDomainVerify {

std::shared_ptr<VerifyHttpTask> VerifyHttpTaskFactory::CreateTask(
    const OHOS::NetStack::HttpClient::HttpClientRequest &httpClientRequest)
{
    std::shared_ptr<VerifyHttpTask> ptr = std::make_shared<VerifyHttpTask>(httpClientRequest);
    if (ptr->GetCurlHandle() == nullptr) {
        return nullptr;
    }
    return ptr;
}
}
}