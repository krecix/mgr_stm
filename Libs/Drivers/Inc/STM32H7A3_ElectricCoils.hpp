//
// Created by krzysztof on 27.03.2021.
//

#ifndef VIBRATIONCONTROLSTM32_STM32H7A3_ELECTRICCOILS_HPP
#define VIBRATIONCONTROLSTM32_STM32H7A3_ELECTRICCOILS_HPP

#include "AbstractElectricCoils.hpp"

#include "stm32h7a3xxq.h"

namespace Drivers {

    class STM32H7A3_ElectricCoils final : public AbstractElectricCoils {
    public:
        explicit STM32H7A3_ElectricCoils();

        void initialize(uint32_t frequency);

        inline void stop() override {
            TIM8->CCR1 = 0UL;
            TIM1->CCR1 = 0UL;
        }

        inline void left(uint32_t duty) override {
            TIM8->CCR1 = duty;
        }

        inline void right(uint32_t duty) override {
            TIM1->CCR1 = duty;
        }

    };
}

#endif //VIBRATIONCONTROLSTM32_STM32H7A3_ELECTRICCOILS_HPP
