# 应用域名校验部件

## 1. 简介

应用域名校验部件属于包管理子系统，提供给包管理子系统使用，实现在应用安装、卸载、更新时同步修改应用和域名的映射关系，在应用间跳转时提供待跳转Ability的过滤。具体功能如下：

### 应用集成

开发者需在module.json5中增加字段标识**domainVerify**为true启用应用域名校验,并在uris增加自己https域名的host
```
{
  "abilities": [
    {
      "skills": [
        {
          "actions": [
            "ohos.want.action.viewData"
          ],
          "entities": [
            "entity.system.browsable"
          ],
          "uris": [
            {
              "scheme":"https",
              "host":"example.com",
              "port":"80",
              "path":"path",
              "type": "text/*"
            }
          ],
          "domainVerify":true
        }
      ]
    }
  ]
}

```
开发者把资产配置文件放置到自己域名的特定路径，资产配置文件中包含应用的载体id、包名、签名指纹信息，资产配置文件路径示例：https://www.xxx.com/.well-known/applinkings.json
内容如下示例
```
{
  "applinking": {
    "apps": [
      {
        "appIdentifier": "载体id",
        "bundleName": "包名",
        "fingerprint": "签名指纹"
      }
    ]
  }
}

```
### 包管理调用
- 应用安装时，调用应用域名校验部件的校验域名接口，获取应用module.json5中声明所有的https域名下的资产配置文件，校验资产配置文件apps数组字段中是否存在项匹配当前应用，如存在则在当前域名下校验成功，否则失败
- 应用卸载时，调用应用域名校验部件的删除接口，删去当前应用的校验结果
- 应用更新时，会先删除当前应用的校验结果，再重新发起校验
- 当应用间使用startAbility进行应用间隐式跳转时，包管理会将Want(隐式跳转，uri为一个https链接)和初筛出的待跳转Ability作为入参，传递给应用域名校验部件，部件根据入参Ability所在应用与Want中uri域名的校验关系缓存，过滤出校验成功的应用的Ability，传回给包管理，完成更精确地应用间跳转

### 周期刷新
- 应用域名校验部件会在设备完成开机后以及固定的时间周期内主动刷新校验失败的应用校验结果


### 架构图

![](figures/architecture_zh.png "应用域名校验部件架构图")

**图 1**  应用域名校验部件架构图

## 2. 代码目录

```
/foundation/bundlemanager/app_domain_verify/
├── etc                      # 组件包含的进程的配置文件
├── figures                  # 架构图
├── interfaces               # 组件对外提供的接口代码
│   └── inner_api            # 内部接口存放目录
├── profile                  # 组件包含的系统服务的配置文件
├── services                 # 应用域名校验服务实现
├── test                     # 测试相关代码
└──README_ZH.md              # 使用说明
```

## 3. 编译构建
在OpenHarmony源码根目录下，调用以下指令，单独编译app_domain_verify。
```shell
./build.sh --product-name rk3568 --ccache --build-target app_domain_verify
```
> **说明：**
--product-name：产品名称，例如Hi3516DV300、rk3568等。
--ccache：编译时使用缓存功能。
--build-target: 编译的部件名称。

## 4. Inner API开发指导
### 4.1 接口说明<a id="inner_api"></a>
#### 4.1.1 app_domain_verify_client.h
接口调用需要引入以下头文件
```
#include "app_domain_verify_client.h"
```
|接口|说明|
|---|---|
|VerifyDomain(const std::string &appIdentifier, const std::string &bundleName, const std::string &fingerprint, const std::vector<SkillUri> &skillUris)：void|提供给BundleManagerService使用，在应用安装和更新时调用，用于触发应用域名校验任务。|
|ClearDomainVerifyStatus(const std::string &appIdentifier, const std::string &bundleName)：bool|提供给BundleManagerService使用，在应用卸载时调用，用于清理被卸载应用的校验信息。返回值用于判断是否成功清理。|
|FilterAbilities(const OHOS::AAFwk::Want &want, const std::vector<OHOS::AppExecFwk::AbilityInfo> &originAbilityInfos, std::vector<OHOS::AppExecFwk::AbilityInfo> &filtedAbilityInfos)：bool|提供给BundleManagerService使用，当应用间使用startAbility进行应用间隐式跳转时，从入参AbilityInfos中筛选出所在应用与want中https域名校验成功的部分。返回值用于判断是否筛选成功。| 

#### 4.1.2 skill_uri.h
skill_uri 信息结构体。

|属性|类型|描述|
|----|----|----|
| scheme | std::string | URI的协议名部分。        |
| host | std::string | URI的主机地址部分。    |
| port  | std::string | URI的端口部分。  |
| path| std::string | URI的路径部分，path、pathStartWith和pathRegex配置时三选一。  |
| pathStartWith| std::string |  URI的路径部分，path、pathStartWith和pathRegex配置时三选一。  |
| pathRegex| std::string |  URI的路径部分，path、pathStartWith和pathRegex配置时三选一。  |
| type| std::string | 标识与Want相匹配的数据类型，使用MIME类型规范。  |

### 4.2 开发步骤
### 在bundle.json中加入依赖：
```
"deps": {
  "components": [
    "app_domain_verify"
]
```
### 在模块gn文件中加入对客户端模块的依赖：
```
external_deps = [
  "app_domain_verify:app_domain_verify_client",
  "app_domain_verify:app_domain_verify_common"
]
```
### 在头文件中引入客户端的头文件：
```
#include "app_domain_verify_client.h"
```
### 发起接口调用
参考[4.1 接口说明](#inner_api)调用接口