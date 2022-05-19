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

#include "res_sched_service.h"
#include <cinttypes>
#include "res_sched_log.h"
#include "res_sched_mgr.h"


namespace OHOS {
namespace ResourceSchedule {
void ResSchedService::ReportData(uint32_t resType, int64_t value, const Json::Value& payload)
{
    RESSCHED_LOGI("ResSchedService::ReportData from ipc receive data resType = %{public}u, value = %{public}lld.",
        resType, (long long)value);
    ResSchedMgr::GetInstance().ReportData(resType, value, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS

