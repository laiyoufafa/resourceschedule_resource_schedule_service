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

#ifndef OHOS_RESOURCE_SCHEDULE_SERVICE_CGROUP_SCHED_FRAMEWORKS_PROCESS_GROUP_INCLUDE_UTIL_H
#define OHOS_RESOURCE_SCHEDULE_SERVICE_CGROUP_SCHED_FRAMEWORKS_PROCESS_GROUP_INCLUDE_UTIL_H

#include <string>

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {

std::string FormatString(const char* fmt, va_list vararg);

std::string StringPrintf(const char* fmt, ...);

bool ReadFileToString(const std::string& fileName, std::string& content);

bool WriteStringToFile(int fd, const std::string& content);

bool WriteStringToFile(const std::string& content, const std::string& fileName);

bool ReadFileToStringForVFS(const std::string& fileName, std::string& content);

} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS

#endif