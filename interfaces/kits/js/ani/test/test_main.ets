/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

import appDomainVerify from '@ohos.bundle.appDomainVerify';

function testQueryAssociatedBundleNames(){
  try{
    let domain = 'https://www.openHarmony.cn'
    let bundleNames = appDomainVerify.queryAssociatedDomains(domain)

    console.log('testQueryAssociatedBundleNames: ' + bundleNames)
  }
  catch (err) {
    console.error('testQueryAssociatedBundleNames failed: ' + err)
  }
}

function testQueryAssociatedDomains(){
  try{
    let bundleName = 'com.openHarmony.test'
    let domains = appDomainVerify.queryAssociatedDomains(bundleName)

    console.log('testQueryAssociatedDomains: ' + domains)
  }
  catch (err) {
    console.error('testQueryAssociatedDomains failed: ' + err)
  }
}

function main() {
  testQueryAssociatedBundleNames();
  testQueryAssociatedDomains();
}
