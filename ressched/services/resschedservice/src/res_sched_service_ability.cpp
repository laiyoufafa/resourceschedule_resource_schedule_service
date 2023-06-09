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

#include "res_sched_service_ability.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_sched_service.h"
#include "system_ability_definition.h"
#include "cgroup_sched.h"

namespace OHOS {
namespace ResourceSchedule {
REGISTER_SYSTEM_ABILITY_BY_ID(ResSchedServiceAbility, RES_SCHED_SYS_ABILITY_ID, true);

void ResSchedServiceAbility::OnStart()
{
    ResSchedMgr::GetInstance().Init();
    if (!service_) {
        try {
            service_ = new ResSchedService();
        } catch(const std::bad_alloc &e) {
            RESSCHED_LOGE("ResSchedServiceAbility:: new ResSchedService failed.");
        }
    }
    if (!Publish(service_)) {
        RESSCHED_LOGE("ResSchedServiceAbility:: Register service failed.");
    }
    CgroupSchedInit();
    AddSystemAbilityListener(APP_MGR_SERVICE_ID);
    AddSystemAbilityListener(WINDOW_MANAGER_SERVICE_ID);
    AddSystemAbilityListener(BACKGROUND_TASK_MANAGER_SERVICE_ID);
    RESSCHED_LOGI("ResSchedServiceAbility ::OnStart.");
}

void ResSchedServiceAbility::OnStop()
{
    RemoveSystemAbilityListener(BACKGROUND_TASK_MANAGER_SERVICE_ID);
    RemoveSystemAbilityListener(WINDOW_MANAGER_SERVICE_ID);
    RemoveSystemAbilityListener(APP_MGR_SERVICE_ID);
    ResSchedMgr::GetInstance().Stop();
    CgroupSchedDeinit();
    RESSCHED_LOGI("ResSchedServiceAbility::OnStop!");
}

void ResSchedServiceAbility::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    ReportAbilityStatus(systemAbilityId, deviceId, 1);
}

void ResSchedServiceAbility::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    ReportAbilityStatus(systemAbilityId, deviceId, 0);
}
} // namespace ResourceSchedule
} // namespace OHOS

