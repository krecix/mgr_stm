//
// Created by krzysztof on 03.05.2021.
//

#ifndef VIBRATIONCONTROLSTM32_ABSTRACTADC_HPP
#define VIBRATIONCONTROLSTM32_ABSTRACTADC_HPP

#include <stdint.h>
#include <stddef.h>

#include "etl_profile.h"
#include "etl/delegate.h"

enum class ReceiveADCValueType_t {
    Actual,
    Buffer
};

struct ADCCallbackConfig_t {
    bool actualValueCallbackEnabled;
    bool bufferCallbackEnabled;
};

struct AbstractADC {
    virtual void init() = 0;

    virtual void start(uint16_t *buffer, size_t maxLength, uint16_t *actualValuePtr,
                       ADCCallbackConfig_t callbackConfig, etl::delegate<void(ReceiveADCValueType_t, size_t)> callback) = 0;

    virtual void stop() = 0;
};

#endif //VIBRATIONCONTROLSTM32_ABSTRACTADC_HPP
