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

#include "res_sched_mgr.h"
#include <cinttypes>
#include "cgroup_sched.h"
#include "res_sched_log.h"
#include "plugin_mgr.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace AppExecFwk;

namespace {
    const std::string RSS_THREAD_NAME = "rssMain";
}

IMPLEMENT_SINGLE_INSTANCE(ResSchedMgr);

void ResSchedMgr::Init()
{
    PluginMgr::GetInstance().Init();

    if (!mainHandler_) {
        mainHandler_ = std::make_shared<EventHandler>(EventRunner::Create(RSS_THREAD_NAME));
    }
}

void ResSchedMgr::Stop()
{
    PluginMgr::GetInstance().Stop();

    std::lock_guard<std::mutex> autoLock(mainHandlerMutex_);
    if (mainHandler_) {
        mainHandler_->RemoveAllEvents();
        mainHandler_ = nullptr;
    }
}

void ResSchedMgr::ReportData(uint32_t resType, int64_t value, const Json::Value& payload)
{
    if (!mainHandler_) {
        return;
    }
    // dispatch resource async
    std::lock_guard<std::mutex> autoLock(mainHandlerMutex_);
    mainHandler_->PostTask([this, resType, value, payload] {
        DispatchResourceInner(resType, value, payload);
        PluginMgr::GetInstance().DispatchResource(std::make_shared<ResData>(resType, value, payload));
    });
}

void ResSchedMgr::DispatchResourceInner(uint32_t resType, int64_t value, const Json::Value& payload)
{
    CgroupSchedDispatch(resType, value, payload);
}

extern "C" void ReportDataInProcess(uint32_t resType, int64_t value, const Json::Value& payload)
{
    RESSCHED_LOGI("ResSchedMgr::ReportDataInProcess receive resType = %{public}u, value = %{public}" PRId64".",
        resType, value);
    ResSchedMgr::GetInstance().ReportData(resType, value, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
