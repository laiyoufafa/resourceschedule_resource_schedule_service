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

#ifndef RES_SCHED_UTILS_H
#define RES_SCHED_UTILS_H

#include <iostream>
#include "sys/types.h"

namespace OHOS {
namespace ResourceSchedule {

using ReportDataFunc = void (*)(uint32_t resType, int64_t value, const std::string& payload);

class ResSchedUtils {
public:
    static ResSchedUtils& GetInstance();
    void ReportDataInProcess(uint32_t resType, int64_t value, const std::string& payload);

private:
    ResSchedUtils()
    {
        if (reportFunc_ == nullptr) {
            LoadUtils();
        }
    }
    ~ResSchedUtils()
    {
        reportFunc_ = nullptr;
    }
    void LoadUtils();

    ResSchedUtils(const ResSchedUtils&) = delete;
    ResSchedUtils& operator=(const ResSchedUtils &) = delete;
    ResSchedUtils(ResSchedUtils&&) = delete;
    ResSchedUtils& operator=(ResSchedUtils&&) = delete;

    ReportDataFunc reportFunc_ = nullptr;
};

} // namespace ResourceSchedule
} // namespace OHOS
#endif // RES_SCHED_UTILS_H 
