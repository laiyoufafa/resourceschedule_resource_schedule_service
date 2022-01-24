/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "window_state_observer.h"

#include "cgroup_sched_log.h"
#include "cgroup_event_handler.h"
#include "res_type.h"
#include "sched_controller.h"

namespace OHOS {
namespace ResourceSchedule {

void WindowStateObserver::OnFocused(uint32_t windowId, sptr<IRemoteObject> abilityToken, WindowType windowType, int32_t displayId)
{
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander != nullptr) {
        cgHander->PostTask([cgHander, windowId, abilityToken, windowType, displayId] {
            cgHander->HandleFocusedWindow(windowId, abilityToken, windowType, displayId);
        });
    }
}

void WindowStateObserver::OnUnfocused(uint32_t windowId, sptr<IRemoteObject> abilityToken, WindowType windowType, int32_t displayId)
{
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander != nullptr) {
        cgHander->PostTask([cgHander, windowId, abilityToken, windowType, displayId] {
            cgHander->HandleUnfocusedWindow(windowId, abilityToken, windowType, displayId);
        });
    }
}

} // namespace OHOS
} // namespace ResourceSchedule
