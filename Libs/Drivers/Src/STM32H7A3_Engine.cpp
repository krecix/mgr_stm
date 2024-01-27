#include "STM32H7A3_Engine.hpp"

#include "stm32h7a3xxq.h"

#include <cmath>

STM32H7A3_Engine::STM32H7A3_Engine(AbstractGPIO &dirLeft_Pin, AbstractGPIO &dirRight_Pin) :
        _dirLeft(dirLeft_Pin),
        _dirRight(dirRight_Pin) {
    stop();
}

void STM32H7A3_Engine::initialize(uint32_t frequency) {
    TIM2->ARR = 999UL;

    auto prescalerFloating = SystemCoreClock / (1000.0 * frequency);
    prescalerFloating = prescalerFloating - 1.0;
    auto psc = static_cast<uint32_t>( round(prescalerFloating));
    TIM2->PSC = psc;

    TIM2->CCR1 = 1000UL;

    // Auto-reload preload buffer
    TIM2->CR1 |= TIM_CR1_ARPE;
    // PWM mode 2, OC Preload enable
    TIM2->CCMR1 |= (7UL << 4UL) | TIM_CCMR1_OC1PE;

    // Enable output
    TIM2->CCER |= TIM_CCER_CC1E;
    TIM2->CR1 |= TIM_CR1_CEN;

}