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

#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_ABILITY_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_ABILITY_H

#include "system_ability.h"
#include "res_sched_service.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedServiceAbility : public SystemAbility {
    DECLARE_SYSTEM_ABILITY(ResSchedServiceAbility);

public:
    ResSchedServiceAbility(int32_t sysAbilityId, bool runOnCreate) : SystemAbility(sysAbilityId, runOnCreate) {}
    ~ResSchedServiceAbility() override = default;

private:
    void OnStart() override;

    void OnStop() override;

    sptr<ResSchedService> service_;

    DISALLOW_COPY_AND_MOVE(ResSchedServiceAbility);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_ABILITY_H