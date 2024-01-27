//
// Created by krzysztof on 03.05.2021.
//

#include "../Inc/STM32H7A3_ADC.hpp"

#include "stm32h7a3xxq.h"

#include "Config.hpp"

namespace Drivers {
    void STM32H7A3_ADC::init() {
        uint32_t arrValue = static_cast<uint32_t>((SystemCoreClock / 275.0 / SAMPLE_FREQUENCY)) - 1;
        TIM3->PSC = 274;
        TIM3->ARR = arrValue;
        TIM3->CR2 |= TIM_CR2_MMS_1;

        // ADC Prescaler /8 (36'666'665Hz / 2) = 18'333'333Hz
        ADC12_COMMON->CCR = (1UL << 18U);

        ADC2->CR &= ~ADC_CR_DEEPPWD;
        ADC2->CR |= ADC_CR_BOOST;

        // Hardware Trigger with detection on the rising edge
        ADC2->CFGR |= ADC_CFGR_EXTEN_0;
        // TIM3_TRGO
        ADC2->CFGR |= ADC_CFGR_EXTSEL_2;
        // Data Management DMA Circular Mode selected
        ADC2->CFGR |= ADC_CFGR_DMNGT;

        // Overwritte when overrun is detected
        ADC2->CFGR |= ADC_CFGR_OVRMOD;

        ADC2->PCSEL |= ADC_PCSEL_PCSEL_1;
        // Sample Rate 18'333'333 / 16.5 = 1MHz 111'111Hz
        ADC2->SMPR1 |= (3UL << 3U);

        //Enable regular oversampling
        ADC2->CFGR2 |= ADC_CFGR2_ROVSE;

        //Oversampling x8
        ADC2->CFGR2 |= ((7UL - 1UL) << 16UL);

        //Shift from 19bits to 16bits
        ADC2->CFGR2 |= (3UL << 5UL);

        auto priority = NVIC_EncodePriority(0x00, 6, 0);
        NVIC_SetPriority(ADC_IRQn, priority);
        NVIC_EnableIRQ(ADC_IRQn);
        ADC2->SQR1 = (1UL << 6U);

        // ADCVREG_StartTime = 10us
        ADC2->CR |= ADC_CR_ADVREGEN;
        auto wait_loop_index = 275'000'000 / (100000UL * 2UL);
        while (wait_loop_index != 0UL) {
            wait_loop_index--;
        }
        // ADC VREG
        if (!(ADC2->CR & ADC_CR_ADVREGEN)) {
            while (true);
        }

        // ADC LDORDY
        while (!(ADC2->ISR & (1UL << 12U)));

        // Kalibracja ADC
        ADC2->CR |= ADC_CR_ADDIS;
        while (ADC2->CR & ADC_CR_ADEN);
        // Linearity calibration
        ADC2->CR |= ADC_CR_ADCALLIN;
        // Uruchomienie i odczekanie na zakończenie kalibracji
        ADC2->CR |= ADC_CR_ADCAL;
        while (ADC2->CR & ADC_CR_ADCAL);


        ADC2->CR |= ADC_CR_ADEN;

        while (!(ADC2->ISR & ADC_ISR_ADRDY));

        ADC2->CR |= ADC_CR_ADSTART;

        TIM3->CR1 |= TIM_CR1_CEN;
    }

    void STM32H7A3_ADC::start(uint16_t *buffer, size_t maxLength, uint16_t *actualValuePtr,
                              ADCCallbackConfig_t callbackConfig,
                              etl::delegate<void(ReceiveADCValueType_t, size_t)> callback) {

        maxLen = maxLength;

        if (callbackConfig.bufferCallbackEnabled || callbackConfig.actualValueCallbackEnabled) {
            m_callback = callback;
        }

        //Actual value circular mode

        // Priority Very High, memory data size 16bit, peripherial data size 16bit,
        // memory increment mode Circural mode
        DMA1_Stream0->CR = DMA_SxCR_PL | DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_CIRC;
        if (callbackConfig.actualValueCallbackEnabled) {
            DMA1_Stream0->CR |= DMA_SxCR_TCIE;
        }
        // x data DMA
        DMA1_Stream0->NDTR = 1;
        DMA1_Stream0->M0AR = (uint32_t) actualValuePtr;
        DMA1_Stream0->PAR = (uint32_t) &ADC2->DR;
        // DMAMUX Request ADC2 -> 10
        DMAMUX1_Channel0->CCR = 10;
        DMAMUX1_ChannelStatus->CFR = DMAMUX_CFR_CSOF0; // I took this from HAL example

        //Buffer moder

        // Priority High, memory data size 16bit, peripherial data size 16bit, memory
        DMA1_Stream1->CR = DMA_SxCR_PL_1 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC;
        if (callbackConfig.bufferCallbackEnabled) {
            DMA1_Stream1->CR |= DMA_SxCR_TCIE;
        }
        // x data DMA
        DMA1_Stream1->NDTR = maxLen;
        DMA1_Stream1->M0AR = (uint32_t) &buffer[0];
        DMA1_Stream1->PAR = (uint32_t) &ADC2->DR;
        // DMAMUX Request ADC2
        DMAMUX1_Channel1->CCR = 10;
        DMAMUX1_ChannelStatus->CFR = DMAMUX_CFR_CSOF1; // I took this from HAL example


        DMA1_Stream0->CR |= DMA_SxCR_EN;
        DMA1_Stream1->CR |= DMA_SxCR_EN;
    }

    void STM32H7A3_ADC::stop() {
        DMA1_Stream0->CR &= ~DMA_SxCR_EN;
        DMA1_Stream1->CR &= ~DMA_SxCR_EN;
    }

    size_t STM32H7A3_ADC::getActual_NDTR_ADC() {

        return DMA1_Stream1->NDTR;
    }
}



