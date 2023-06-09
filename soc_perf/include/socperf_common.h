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

#ifndef SOC_PERF_INCLUDE_SOCPERF_COMMON_H
#define SOC_PERF_INCLUDE_SOCPERF_COMMON_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "hilog/log.h"

namespace OHOS {
namespace SOCPERF {
#define LOG_TAG_SOC_PERF "socperf"
#define LOG_TAG_DOMAIN_ID_SOC_PERF 0xD001700

static constexpr OHOS::HiviewDFX::HiLogLabel SOC_PERF_LOG_LABEL = {
    LOG_CORE,
    LOG_TAG_DOMAIN_ID_SOC_PERF,
    LOG_TAG_SOC_PERF
};

#define SOC_PERF_LOGF(...) (void)OHOS::HiviewDFX::HiLog::Fatal(SOC_PERF_LOG_LABEL, __VA_ARGS__)
#define SOC_PERF_LOGE(...) (void)OHOS::HiviewDFX::HiLog::Error(SOC_PERF_LOG_LABEL, __VA_ARGS__)
#define SOC_PERF_LOGW(...) (void)OHOS::HiviewDFX::HiLog::Warn(SOC_PERF_LOG_LABEL, __VA_ARGS__)
#define SOC_PERF_LOGI(...) (void)OHOS::HiviewDFX::HiLog::Info(SOC_PERF_LOG_LABEL, __VA_ARGS__)
#define SOC_PERF_LOGD(...) (void)OHOS::HiviewDFX::HiLog::Debug(SOC_PERF_LOG_LABEL, __VA_ARGS__)

enum EventType {
    EVENT_INVALID = -1,
    EVENT_OFF,
    EVENT_ON
};

enum ActionType {
    ACTION_TYPE_PERF,
    ACTION_TYPE_POWER,
    ACTION_TYPE_THERMAL,
    ACTION_TYPE_MAX
};

namespace {
    const int32_t INNER_EVENT_ID_INIT_RES_NODE_INFO       = 0;
    const int32_t INNER_EVENT_ID_INIT_GOV_RES_NODE_INFO   = 1;
    const int32_t INNER_EVENT_ID_DO_FREQ_ACTION           = 2;
    const int32_t INNER_EVENT_ID_DO_FREQ_ACTION_DELAYED   = 3;
    const int32_t INNER_EVENT_ID_POWER_LIMIT_BOOST_FREQ   = 4;
    const int32_t INNER_EVENT_ID_THERMAL_LIMIT_BOOST_FREQ = 5;
}

namespace {
    const std::string SOCPERF_RESOURCE_CONFIG_XML = "/system/etc/socperf/socperf_resource_config.xml";
    const std::string SOCPERF_BOOST_CONFIG_XML    = "/system/etc/socperf/socperf_boost_config.xml";
    const std::string SOCPERF_POWER_CONFIG_XML    = "/system/etc/socperf/socperf_power_config.xml";
    const std::string SOCPERF_THERMAL_CONFIG_XML  = "/system/etc/socperf/socperf_thermal_config.xml";
    const int32_t MAX_INT_VALUE                       = 0x7FFFFFFF;
    const int32_t MIN_INT_VALUE                       = 0x80000000;
    const int32_t INVALID_VALUE                       = -1;
    const int32_t MAX_HANDLER_THREADS                 = 5;
    const int32_t MIN_RESOURCE_ID                     = 1000;
    const int32_t MAX_RESOURCE_ID                     = 5999;
    const int32_t RES_ID_AND_VALUE_PAIR               = 2;
    const int32_t RES_ID_NUMS_PER_TYPE                = 1000;
}

class ResNode {
public:
    int32_t id;
    std::string name;
    int32_t def;
    std::string path;
    int32_t mode;
    int32_t pair;
    std::unordered_set<int32_t> available;

public:
    ResNode(int32_t resId, std::string resName, int32_t resMode, int32_t resPair)
    {
        id = resId;
        name = resName;
        mode = resMode;
        pair = resPair;
        def = INVALID_VALUE;
    }
    ~ResNode() {}

    void PrintString()
    {
        SOC_PERF_LOGI("resNode-> id: [%{public}d], name: [%{public}s]", id, name.c_str());
        SOC_PERF_LOGI("          path: [%{public}s]", path.c_str());
        SOC_PERF_LOGI("          def: [%{public}d], mode: [%{public}d], pair: [%{public}d]", def, mode, pair);
        std::string str;
        str.append("available(").append(std::to_string((int32_t)available.size())).append("): ");
        str.append("[");
        for (auto validValue : available) {
            str.append(std::to_string(validValue)).append(",");
        }
        if (!available.empty()) {
            str.pop_back();
        }
        str.append("]");
        SOC_PERF_LOGI("          %{public}s", str.c_str());
    }
};

class GovResNode {
public:
    int32_t id;
    std::string name;
    int32_t def;
    std::vector<std::string> paths;
    std::unordered_set<int32_t> available;
    std::unordered_map<int32_t, std::vector<std::string>> levelToStr;

public:
    GovResNode(int32_t govResId, std::string govResName)
    {
        id = govResId;
        name = govResName;
        def = INVALID_VALUE;
    }
    ~GovResNode() {}

    void PrintString()
    {
        SOC_PERF_LOGI("govResNode-> id: [%{public}d], name: [%{public}s]", id, name.c_str());
        SOC_PERF_LOGI("             def: [%{public}d]", def);
        for (auto path : paths) {
            SOC_PERF_LOGI("             path: [%{public}s]", path.c_str());
        }
        std::string str;
        str.append("available(").append(std::to_string((int32_t)available.size())).append("): ");
        str.append("[");
        for (auto validValue : available) {
            str.append(std::to_string(validValue)).append(",");
        }
        if (!available.empty()) {
            str.pop_back();
        }
        str.append("]");
        SOC_PERF_LOGI("             %{public}s", str.c_str());
        for (auto iter = levelToStr.begin(); iter != levelToStr.end(); ++iter) {
            std::string str2;
            int32_t level = iter->first;
            std::vector<std::string> result = iter->second;
            for (int32_t i = 0; i < (int32_t)result.size(); i++) {
                str2.append(result[i]).append(",");
            }
            if (!result.empty()) {
                str2.pop_back();
            }
            SOC_PERF_LOGI("             %{public}d: [%{public}s]", level, str2.c_str());
        }
    }
};

class Action {
public:
    int32_t id;
    std::string name;
    int32_t duration;
    std::vector<int32_t> variable;

public:
    Action(int32_t cmdId, std::string cmdName)
    {
        id = cmdId;
        name = cmdName;
        duration = INVALID_VALUE;
    }
    ~Action() {}

    void PrintString()
    {
        SOC_PERF_LOGI("action-> id: [%{public}d], name: [%{public}s]", id, name.c_str());
        SOC_PERF_LOGI("         duration: [%{public}d]", duration);
        std::string str;
        str.append("variable(").append(std::to_string((int32_t)variable.size())).append("): [");
        for (int32_t i = 0; i < (int32_t)variable.size(); i++) {
            str.append(std::to_string(variable[i])).append(",");
        }
        if (!variable.empty()) {
            str.pop_back();
        }
        str.append("]");
        SOC_PERF_LOGI("         %{public}s", str.c_str());
    }
};

class ResAction {
public:
    int32_t value;
    int32_t duration;
    int32_t type;
    int32_t onOff;

public:
    ResAction(int32_t resActionValue, int32_t resActionDuration, int32_t resActionType, int32_t resActionOnOff)
    {
        value = resActionValue;
        duration = resActionDuration;
        type = resActionType;
        onOff = resActionOnOff;
    }
    ~ResAction() {}

    bool TotalSame(std::shared_ptr<ResAction> resAction)
    {
        if (value == resAction->value
            && duration == resAction->duration
            && type == resAction->type
            && onOff == resAction->onOff) {
            return true;
        }
        return false;
    }

    bool PartSame(std::shared_ptr<ResAction> resAction)
    {
        if (value == resAction->value
            && duration == resAction->duration
            && type == resAction->type) {
            return true;
        }
        return false;
    }
};

class ResStatus {
public:
    std::vector<std::list<std::shared_ptr<ResAction>>> resActionList;
    std::vector<int32_t> candidates;
    int32_t candidate;
    int32_t current;

public:
    explicit ResStatus(int32_t val)
    {
        resActionList = std::vector<std::list<std::shared_ptr<ResAction>>>(ACTION_TYPE_MAX);
        candidates = std::vector<int32_t>(ACTION_TYPE_MAX);
        candidates[ACTION_TYPE_PERF] = INVALID_VALUE;
        candidates[ACTION_TYPE_POWER] = INVALID_VALUE;
        candidates[ACTION_TYPE_THERMAL] = INVALID_VALUE;
        candidate = val;
        current = val;
    }
    ~ResStatus() {}

    std::string ToString()
    {
        std::string str;
        str.append("perf:[");
        for (auto iter = resActionList[ACTION_TYPE_PERF].begin();
            iter != resActionList[ACTION_TYPE_PERF].end(); ++iter) {
            str.append(std::to_string((*iter)->value)).append(",");
        }
        if (!resActionList[ACTION_TYPE_PERF].empty()) {
            str.pop_back();
        }
        str.append("]power:[");
        for (auto iter = resActionList[ACTION_TYPE_POWER].begin();
            iter != resActionList[ACTION_TYPE_POWER].end(); ++iter) {
            str.append(std::to_string((*iter)->value)).append(",");
        }
        if (!resActionList[ACTION_TYPE_POWER].empty()) {
            str.pop_back();
        }
        str.append("]thermal:[");
        for (auto iter = resActionList[ACTION_TYPE_THERMAL].begin();
            iter != resActionList[ACTION_TYPE_THERMAL].end(); ++iter) {
            str.append(std::to_string((*iter)->value)).append(",");
        }
        if (!resActionList[ACTION_TYPE_THERMAL].empty()) {
            str.pop_back();
        }
        str.append("]candidates[");
        for (auto iter = candidates.begin(); iter != candidates.end(); ++iter) {
            str.append(std::to_string(*iter)).append(",");
        }
        str.pop_back();
        str.append("]candidate[").append(std::to_string(candidate));
        str.append("]current[").append(std::to_string(current)).append("]");
        return str;
    }
};

static inline int32_t Max(int32_t num1, int32_t num2)
{
    if (num1 >= num2) {
        return num1;
    }
    return num2;
}

static inline int32_t Max(int32_t num1, int32_t num2, int32_t num3)
{
    return Max(Max(num1, num2), num3);
}

static inline int32_t Min(int32_t num1, int32_t num2)
{
    if (num1 <= num2) {
        return num1;
    }
    return num2;
}

static inline int32_t Min(int32_t num1, int32_t num2, int32_t num3)
{
    return Min(Min(num1, num2), num3);
}

static inline bool IsNumber(std::string str)
{
    for (int32_t i = 0; i < (int32_t)str.size(); i++) {
        if (i == 0 && str.at(i) == '-') {
            continue;
        }
        if (str.at(i) < '0' || str.at(i) > '9') {
            return false;
        }
    }
    return true;
}

static inline bool IsValidResId(int32_t id)
{
    if (id < MIN_RESOURCE_ID || id > MAX_RESOURCE_ID) {
        return false;
    }
    return true;
}

static inline std::vector<std::string> Split(std::string str, std::string pattern)
{
    int32_t position;
    std::vector<std::string> result;
    str += pattern;
    int32_t length = (int32_t)str.size();
    for (int32_t i = 0; i < length; i++) {
        position = (int32_t)str.find(pattern, i);
        if (position < length) {
            std::string tmp = str.substr(i, position - i);
            result.push_back(tmp);
            i = position + (int32_t)pattern.size() - 1;
        }
    }
    return result;
}
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_INCLUDE_SOCPERF_COMMON_H
