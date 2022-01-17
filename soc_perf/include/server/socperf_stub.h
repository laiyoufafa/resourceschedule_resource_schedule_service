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

#ifndef SOC_PERF_STUB_H
#define SOC_PERF_STUB_H

#include "socperf_service.h"

namespace OHOS {
namespace SOCPERF {
class SocPerfStub : public IRemoteStub<ISocPerfService> {
public:
    virtual int OnRemoteRequest(uint32_t code, MessageParcel &data,
        MessageParcel &reply, MessageOption &option) override;

public:
    SocPerfStub() {}
    ~SocPerfStub() {}
};
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_STUB_H