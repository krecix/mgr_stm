//
// Created by krzysztof on 27.03.2021.
//

#include "STM32H7A3_ElectricCoils.hpp"

#include <cmath>

namespace Drivers {

    STM32H7A3_ElectricCoils::STM32H7A3_ElectricCoils() = default;

    void STM32H7A3_ElectricCoils::initialize(uint32_t frequency) {

        //Left coil TIM8 Channel 1
        TIM8->ARR = 999UL;
        auto prescalerFloating = static_cast<double>(SystemCoreClock) / ((TIM8->ARR + 1.0) * frequency);
        prescalerFloating = prescalerFloating - 1.0f;
        auto psc = static_cast<uint32_t>( round(prescalerFloating));
        TIM8->PSC = psc;
        TIM8->CCR1 = 0UL;
        // Auto-reload preload buffer
        TIM8->CR1 |= TIM_CR1_ARPE;
        // PWM mode 2, OC Preload enable
        TIM8->CCMR1 |= (7UL << 4UL) | TIM_CCMR1_OC1PE;
        //Main output enable
        TIM8->BDTR |= TIM_BDTR_MOE;
        // Enable output
        TIM8->CCER |= TIM_CCER_CC1E;
        TIM8->CR1 |= TIM_CR1_CEN;

        //Right coil TIM1 Channel 1
        TIM1->ARR = 999UL;
        prescalerFloating = static_cast<double>(SystemCoreClock) / ((TIM1->ARR + 1.0) * frequency);
        prescalerFloating = prescalerFloating - 1.0f;
        psc = static_cast<uint32_t>( round(prescalerFloating));
        TIM1->PSC = psc;
        TIM1->CCR1 = 0UL;
        // Auto-reload preload buffer
        TIM1->CR1 |= TIM_CR1_ARPE;
        // PWM mode 2, OC Preload enable
        TIM1->CCMR1 |= (7UL << 4UL) | TIM_CCMR1_OC1PE;
        //Main output enable
        TIM1->BDTR |= TIM_BDTR_MOE;
        // Enable output
        TIM1->CCER |= TIM_CCER_CC1E;
        TIM1->CR1 |= TIM_CR1_CEN;
    }

}

