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

#ifndef CGROUP_ADJUSTER_H
#define CGROUP_ADJUSTER_H

#include <sys/types.h>

#include "supervisor.h"

namespace OHOS {
namespace ResourceSchedule {

enum class AdjustSource {
    ADJS_BEGIN = 0,
    ADJS_FG_APP_CHANGE = ADJS_BEGIN,
    ADJS_ABILITY_STATE,
    ADJS_EXTENSION_STATE,
    ADJS_PROCESS_CREATE,
    ADJS_TRANSIENT_BEGIN,
    ADJS_TRANSIENT_END,
    ADJS_CONTINUOUS_BEGIN,
    ADJS_CONTINUOUS_END,
    ADJS_FOCUSED_WINDOW,
    ADJS_UNFOCUSED_WINDOW,
    ADJS_END
};

class CgroupAdjuster {
public:
    CgroupAdjuster() = default;
    ~CgroupAdjuster() = default;
    void InitAdjuster();
    void AdjustProcessGroup(Application &app, ProcessRecord &pr, AdjustSource source);
    void AdjustAllProcessGroup(Application &app, AdjustSource source);
    
private:
    void ComputeProcessGroup(Application &app, ProcessRecord &pr, AdjustSource source);
    void ApplyProcessGroup(ProcessRecord &pr);
    inline void AdjustSelfProcessGroup();
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_ADJUSTER_H