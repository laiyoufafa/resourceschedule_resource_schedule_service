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

#include "frame_aware_plugin.h"
#include "frame_msg_intf.h"
#include "res_type.h"
#include "res_sched_log.h"
#include "plugin_mgr.h"
#include "config_info.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libframe_aware_plugin.z.so";
}
IMPLEMENT_SINGLE_INSTANCE(FrameAwarePlugin)

void FrameAwarePlugin::Init()
{
    netLatCtrl.Init();
    functionMap = {
        { RES_TYPE_APP_STATE_CHANGE,
            [this](const std::shared_ptr<ResData>& data) { HandleAppStateChange(data); } },
        { RES_TYPE_PROCESS_STATE_CHANGE,
            [this](const std::shared_ptr<ResData>& data) { HandleProcessStateChange(data); } },
        { RES_TYPE_CGROUP_ADJUSTER,
            [this](const std::shared_ptr<ResData>& data) { HandleCgroupAdjuster(data); } },
        { RES_TYPE_WINDOW_FOCUS,
            [this](const std::shared_ptr<ResData>& data) { HandleWindowsFocus(data); } },
        { RES_TYPE_REPORT_RENDER_THREAD,
            [this](const std::shared_ptr<ResData>& data) { HandleReportRender(data); } },
        { RES_TYPE_NETWORK_LATENCY_REQUEST,
            [this](const std::shared_ptr<ResData>& data) { HandleNetworkLatencyRequest(data); } },
    };
    resTypes = {
        RES_TYPE_APP_STATE_CHANGE,
        RES_TYPE_PROCESS_STATE_CHANGE,
        RES_TYPE_CGROUP_ADJUSTER,
        RES_TYPE_WINDOW_FOCUS,
        RES_TYPE_REPORT_RENDER_THREAD,
        RES_TYPE_NETWORK_LATENCY_REQUEST,
    };
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    RME::FrameMsgIntf::GetInstance().Init();
    RESSCHED_LOGI("FrameAwarePlugin::Init ueaServer success");
}

void FrameAwarePlugin::Disable()
{
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    RME::FrameMsgIntf::GetInstance().Stop();
    functionMap.clear();
    resTypes.clear();
    RESSCHED_LOGI("FrameAwarePlugin::Disable ueaServer success");
}

void FrameAwarePlugin::HandleAppStateChange(const std::shared_ptr<ResData>& data)
{
    Json::Value payload = data->payload;
    int uid = atoi(payload["uid"].asString().c_str());
    int pid = atoi(payload["pid"].asString().c_str());
    std::string bundleName = payload["bundleName"].asString().c_str();
    RME::ThreadState state = static_cast<RME::ThreadState>(data->value);
    RME::FrameMsgIntf::GetInstance().ReportAppInfo(pid, uid, bundleName, state);
}

void FrameAwarePlugin::HandleProcessStateChange(const std::shared_ptr<ResData>& data)
{
    Json::Value payload = data->payload;
    int pid = atoi(payload["pid"].asString().c_str());
    int uid = atoi(payload["uid"].asString().c_str());
    std::string bundleName = payload["bundleName"].asString().c_str();
    RME::ThreadState state = static_cast<RME::ThreadState>(data->value);
    RME::FrameMsgIntf::GetInstance().ReportProcessInfo(pid, uid, bundleName, state);
}

void FrameAwarePlugin::HandleCgroupAdjuster(const std::shared_ptr<ResData>& data)
{
    Json::Value payload = data->payload;
    int pid = atoi(payload["pid"].asString().c_str());
    int uid = atoi(payload["uid"].asString().c_str());
    int oldGroup = atoi(payload["oldGroup"].asString().c_str());
    int newGroup = atoi(payload["newGroup"].asString().c_str());
    if (!data->value) {
        RME::FrameMsgIntf::GetInstance().ReportCgroupChange(pid, uid, oldGroup, newGroup);
    }
}

void FrameAwarePlugin::HandleWindowsFocus(const std::shared_ptr<ResData>& data)
{
    Json::Value payload = data->payload;
    int pid = atoi(payload["pid"].asString().c_str());
    int uid = atoi(payload["uid"].asString().c_str());
    RME::FrameMsgIntf::GetInstance().ReportWindowFocus(pid, uid, data->value);
}

void FrameAwarePlugin::HandleReportRender(const std::shared_ptr<ResData>& data)
{
    Json::Value payload = data->payload;
    int pid = atoi(payload["pid"].asString().c_str());
    int uid = atoi(payload["uid"].asString().c_str());
    RME::FrameMsgIntf::GetInstance().ReportRenderThread(pid, uid, data->value);
}

void FrameAwarePlugin::HandleNetworkLatencyRequest(const std::shared_ptr<ResData>& data)
{
    Json::Value payload = data->payload;
    long long value = data->value;
    std::string identity = payload["identity"].asString();
    netLatCtrl.HandleRequest(value, identity);
}

void FrameAwarePlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("FrameAwarePlugin:DispatchResource type:%{public}u, value:%{public}lld",
                  data->resType, (long long)data->value);
    auto funcIter = functionMap.find(data->resType);
    if (funcIter != functionMap.end()) {
        auto function = funcIter->second;
        if (function) {
            function(data);
        }
    }
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        RESSCHED_LOGE("FrameAwarePlugin::OnPluginInit lib name is not match");
        return false;
    }
    FrameAwarePlugin::GetInstance().Init();
    RESSCHED_LOGI("FrameAwarePlugin::OnPluginInit success.");
    return true;
}

extern "C" void OnPluginDisable()
{
    FrameAwarePlugin::GetInstance().Disable();
    RESSCHED_LOGI("FrameAwarePlugin::OnPluginDisable success.");
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    FrameAwarePlugin::GetInstance().DispatchResource(data);
    RESSCHED_LOGI("FrameAwarePlugin::OnDispatchResource success.");
}
}
}
