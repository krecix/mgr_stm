//
// Created by krzysztof on 03.05.2021.
//

#ifndef VIBRATIONCONTROLSTM32_STM32H7A3_ADC_HPP
#define VIBRATIONCONTROLSTM32_STM32H7A3_ADC_HPP

#include "AbstractADC.hpp"

namespace Drivers {
    class STM32H7A3_ADC final : public AbstractADC {
    public:
        void init() override;

        void start(uint16_t *buffer, size_t maxLength, uint16_t *actualValuePtr, ADCCallbackConfig_t callbackConfig,
                   etl::delegate<void(ReceiveADCValueType_t, size_t)> callback) override;

        void stop() override;

        //Actual value circular mode
        inline void DMA1_Stream0_Interrupt() {
            m_callback(ReceiveADCValueType_t::Actual, 1);
        }

        //Buffer mode
        inline void DMA1_Stream1_Interrupt() {
            int b = maxLen;
            int a = getActual_NDTR_ADC();
            int c = maxLen - getActual_NDTR_ADC();
            //m_callback(ReceiveADCValueType_t::Buffer, maxLen - getActual_NDTR_ADC());
            m_callback(ReceiveADCValueType_t::Buffer, 12000);
        }

    private:

        size_t getActual_NDTR_ADC();

        etl::delegate<void(ReceiveADCValueType_t, size_t length)> m_callback;
        size_t maxLen;
    };
}

#endif //VIBRATIONCONTROLSTM32_STM32H7A3_ADC_HPP
