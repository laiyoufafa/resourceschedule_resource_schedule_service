/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "plugin_mgr.h"
#include <algorithm>
#include <dlfcn.h>
#include <iostream>
#include "res_sched_log.h"
#include "event_runner.h"
#include "datetime_ex.h"

using namespace std;

namespace OHOS {
namespace ResourceSchedule {

using namespace AppExecFwk;

using OnPluginInitFunc = bool (*)(std::string);

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace {
    const int DISPATCH_WARNING_TIME = 1; // ms
    const int DISPATCH_TIME_OUT = 10; // ms
    const std::string RUNNER_NAME = "rssDispatcher";
    const std::string PLUGIN_SWITCH_FILE_NAME = "/system/etc/ressched/res_sched_plugin_switch.xml";
    const std::string CONFIG_FILE_NAME = "/system/etc/ressched/res_sched_config.xml";
}

IMPLEMENT_SINGLE_INSTANCE(PluginMgr);

PluginMgr::~PluginMgr()
{
    OnDestroy();
}

void PluginMgr::Init()
{
    RESSCHED_LOGI("PluginMgr::Init enter!");
    if (pluginSwitch_ != nullptr) {
        RESSCHED_LOGW("PluginMgr::Init has Initialized!");
        return;
    }

    if (pluginSwitch_ == nullptr) {
        pluginSwitch_ = make_unique<PluginSwitch>();
        bool loadRet = pluginSwitch_->LoadFromConfigFile(PLUGIN_SWITCH_FILE_NAME);
        if (!loadRet) {
            RESSCHED_LOGW("PluginMgr::Init load switch config file failed!");
        }
    }

    if (configReader_ == nullptr) {
        configReader_ = make_unique<ConfigReader>();
        bool loadRet = configReader_->LoadFromCustConfigFile(CONFIG_FILE_NAME);
        if (!loadRet) {
            RESSCHED_LOGW("PluginMgr::Init load config file failed!");
        }
    }

    LoadPlugin();

    if (dispatcherHandler_ == nullptr) {
        dispatcherHandler_ = std::make_shared<EventHandler>(EventRunner::Create(RUNNER_NAME));
    }
    RESSCHED_LOGI("PluginMgr::Init success!");
}

void PluginMgr::LoadPlugin()
{
    if (pluginSwitch_ == nullptr) {
        RESSCHED_LOGW("PluginMgr::LoadPlugin configReader null!");
        return;
    }

    std::list<PluginInfo> pluginInfoList = pluginSwitch_->GetPluginSwitch();
    for (const auto& info : pluginInfoList) {
        if (!info.switchOn) {
            continue;
        }

        auto pluginHandle = dlopen(info.libPath.c_str(), RTLD_NOW);
        if (pluginHandle == nullptr) {
            RESSCHED_LOGE("PluginMgr::LoadPlugin not find plugin lib !");
            continue;
        }

        auto onPluginInitFunc = reinterpret_cast<OnPluginInitFunc>(dlsym(pluginHandle, "OnPluginInit"));
        if (onPluginInitFunc == nullptr) {
            RESSCHED_LOGE("PluginMgr::LoadPlugin dlsym OnPluginInit failed!");
            dlclose(pluginHandle);
            continue;
        }

        auto onPluginDisableFunc = reinterpret_cast<OnPluginDisableFunc>(dlsym(pluginHandle, "OnPluginDisable"));
        if (onPluginDisableFunc == nullptr) {
            RESSCHED_LOGE("PluginMgr::LoadPlugin dlsym OnPluginDisable failed!");
            dlclose(pluginHandle);
            continue;
        }

        if (!onPluginInitFunc(info.libPath)) {
            RESSCHED_LOGE("PluginMgr::LoadPlugin plugin init failed!");
            dlclose(pluginHandle);
            continue;
        }

        // OnDispatchResource is not necessary for plugin
        auto onDispatchResourceFunc = reinterpret_cast<OnDispatchResourceFunc>(dlsym(pluginHandle, "OnDispatchResource"));

        // shared_ptr save handle pointer and ensure handle close correctly
        shared_ptr<void> sharedPluginHandle(pluginHandle, CloseHandle);

        PluginLib libInfo;
        libInfo.handle = sharedPluginHandle;
        libInfo.onDispatchResourceFunc_ = onDispatchResourceFunc;
        libInfo.onPluginDisableFunc_ = onPluginDisableFunc;
        {
            std::lock_guard<std::mutex> autoLock(pluginMutex_);
            pluginLibMap_.emplace(info.libPath, libInfo);
        }
        RESSCHED_LOGI("PluginMgr::LoadPlugin init %{public}s success!", info.libPath.c_str());
    }
}

PluginConfig PluginMgr::GetConfig(const std::string& pluginName, const std::string& configName)
{
    PluginConfig config;
    if (configReader_ == nullptr) {
        return config;
    }
    return configReader_->GetConfig(pluginName, configName);
}

void PluginMgr::Stop()
{
    OnDestroy();
}

void PluginMgr::DispatchResource(const std::shared_ptr<ResData>& resData)
{
    if (resData == nullptr) {
        RESSCHED_LOGE("PluginMgr::DispatchResource failed, null res data.");
        return;
    }

    if (dispatcherHandler_ == nullptr) {
        RESSCHED_LOGE("PluginMgr::DispatchResource failed, dispatcher is stopped.");
        return;
    }

    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    auto iter = resTypeLibMap_.find(resData->resType);
    if (iter == resTypeLibMap_.end()) {
        RESSCHED_LOGW("PluginMgr::DispatchResource resType no lib register!");
        return;
    }
    std::string libNameAll = "[";
    for (const auto& libName : iter->second) {
        libNameAll.append(libName);
        libNameAll.append(",");
    }
    libNameAll.append("]");
    RESSCHED_LOGI("PluginMgr::DispatchResource resType = %{public}d, value = %{public}lld pluginlist is %{public}s.",
        resData->resType, resData->value, libNameAll.c_str());
    for (const auto& libName : iter->second) {
        dispatcherHandler_->PostTask([libName = libName, resData, this] { deliverResourceToPlugin(libName, resData); });
    }
}

void PluginMgr::SubscribeResource(const std::string& pluginLib, uint32_t resType)
{
    if (pluginLib.size() == 0) {
        RESSCHED_LOGE("PluginMgr::SubscribeResource failed, pluginLib is null.");
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    resTypeLibMap_[resType].emplace_back(pluginLib);
}

void PluginMgr::UnSubscribeResource(const std::string& pluginLib, uint32_t resType)
{
    if (pluginLib.size() == 0) {
        RESSCHED_LOGE("PluginMgr::UnSubscribeResource failed, pluginLib is null.");
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    auto iter = resTypeLibMap_.find(resType);
    if (iter == resTypeLibMap_.end()) {
        RESSCHED_LOGE("PluginMgr::UnSubscribeResource failed, res type has no plugin subscribe.");
        return;
    }

    iter->second.remove(pluginLib);
    if (iter->second.empty()) {
        resTypeLibMap_.erase(iter);
    }
}

void PluginMgr::ClearResource()
{
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    resTypeLibMap_.clear();
}

void PluginMgr::deliverResourceToPlugin(const std::string& pluginLib, const std::shared_ptr<ResData>& resData)
{
    std::lock_guard<std::mutex> autoLock(pluginMutex_);
    auto itMap = pluginLibMap_.find(pluginLib);
    if (itMap == pluginLibMap_.end()) {
        RESSCHED_LOGE("PluginMgr::deliverResourceToPlugin no plugin %{public}s !", pluginLib.c_str());
        return;
    }
    OnDispatchResourceFunc fun = itMap->second.onDispatchResourceFunc_;
    if (fun == nullptr) {
        RESSCHED_LOGE("PluginMgr::deliverResourceToPlugin no DispatchResourceFun !");
        return;
    }

    auto beginTime = Clock::now();
    try {
        fun(resData);
    }
    catch(...) {
        RESSCHED_LOGE("PluginMgr::%{public}s throw a exception!", pluginLib.c_str());
    }
    auto endTime = Clock::now();
    int costTime = (endTime - beginTime) / std::chrono::milliseconds(1);
    if (costTime > DISPATCH_TIME_OUT) {
        // dispatch resource use too long time, unload it
        RESSCHED_LOGE("PluginMgr::deliverResourceToPlugin ERROR :"
                      "%{public}s plugin cost time(%{public}dms) over 10 ms! disable it.",
                      pluginLib.c_str(), costTime);
        if (itMap->second.onPluginDisableFunc_ != nullptr) {
            itMap->second.onPluginDisableFunc_();
        }
        pluginLibMap_.erase(itMap);
    } else if (costTime > DISPATCH_WARNING_TIME) {
        RESSCHED_LOGW("PluginMgr::deliverResourceToPlugin WAINNING :"
                      "%{public}s plugin cost time(%{public}dms) over 1 ms!",
                      pluginLib.c_str(), costTime);
    }
}

void PluginMgr::UnLoadPlugin()
{
    std::lock_guard<std::mutex> autoLock(pluginMutex_);
    // unload all plugin
    for (const auto& [libPath, libInfo] : pluginLibMap_) {
        if (libInfo.onPluginDisableFunc_ == nullptr) {
            continue;
        }
        libInfo.onPluginDisableFunc_();
    }

    // close all plugin handle
    pluginLibMap_.clear();
}

void PluginMgr::OnDestroy()
{
    UnLoadPlugin();
    configReader_ = nullptr;
    ClearResource();

    if (dispatcherHandler_ != nullptr) {
        dispatcherHandler_->RemoveAllEvents();
        dispatcherHandler_ = nullptr;
    }
}

void PluginMgr::CloseHandle(const DlHandle& handle)
{
    if (handle == nullptr) {
        RESSCHED_LOGW("PluginMgr::CloseHandle nullptr handle!");
        return;
    }

    int ret = dlclose(handle);
    if (ret) {
        RESSCHED_LOGW("PluginMgr::CloseHandle handle close failed!");
    }
}

} // namespace ResourceSchedule
} // namespace OHOS