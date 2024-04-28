//
// Created by krzysztof on 06.04.2021.
//

#include "System.hpp"
#include "CCUHardware.hpp"

#include "stm32h7a3xxq.h"

#include "PIDController.hpp"

extern ccu::CCUHardware hardware;

volatile uint32_t adcData;


void adcInit() {
    RCC->CDCCIPR |= RCC_CDCCIPR_CKPERSEL_1;
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;

    // PC3 Analog
    GPIOC->MODER |= GPIO_MODER_MODE3;

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

    ADC2->IER |= ADC_IER_EOCIE;

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

    //DMA

//    // Priority Very High, memory data size 16bit, peripherial data size 16bit,
//    // memory increment mode Circural mode
//    DMA1_Stream0->CR = DMA_SxCR_PL | DMA_SxCR_MSIZE_1 | DMA_SxCR_PSIZE_1 | DMA_SxCR_MINC | DMA_SxCR_CIRC | DMA_SxCR_TCIE;
//
//    // x data DMA
//    DMA1_Stream0->NDTR = 1;
//
//    DMA1_Stream0->M0AR = (uint32_t)&adcData;
//    DMA1_Stream0->PAR = (uint32_t)&ADC2->DR;
//
//    // DMAMUX Request ADC2 -> 10
//    DMAMUX1_Channel0->CCR = 10;
//    DMAMUX1_ChannelStatus->CFR = DMAMUX_CFR_CSOF0; // I took this from HAL example
//
//    DMA1_Stream0->CR |= DMA_SxCR_EN;

}


PIDController<float, int32_t>::PIDParameters pidParameters{
        .Kp = -0.9,
        .Ki = -1.8,
        .Kd = 4.6,
        .antiWindupLimit = 1000
};

int32_t setpoint = 0;
int32_t adcValue = 0;
int32_t amplitude = 0;
int32_t pidResponse = 0;

PIDController<float, int32_t> pidController(pidParameters);

void pidOperate() {
    adcValue = static_cast<int32_t >(adcData);

    amplitude = adcValue - setpoint;

    pidResponse = pidController.calculate(0, amplitude);

    if (pidResponse < 0) {
//        hardware.ElectricCoilsUnit.left(0);
//        hardware.ElectricCoilsUnit.right(-pidResponse);

    } else {
//        hardware.ElectricCoilsUnit.right(0);
//        hardware.ElectricCoilsUnit.left(pidResponse);

    }
}

[[noreturn]] void pidTaskProcedure(void *) {

    //500 Hz
    TIM3->PSC = 274;
    TIM3->ARR = 1999;
    TIM3->CR2 |= TIM_CR2_MMS_1;

    adcInit();

    TIM3->CR1 |= TIM_CR1_CEN;


    size_t measureCount = 0;
    uint32_t sum = 0;
    while (true) {
        sum += adcData;
        measureCount++;

        if (measureCount == 1000) {
            break;
        }

        osDelay(System::TicksFromMs(1));
    }
    setpoint = static_cast<int32_t>(sum / 1000.0);

    while (true) {
        osDelay(System::TicksFromMs(1));
    }
}

extern "C" void ADC_IRQHandler() {

    if ((ADC2->ISR & ADC_ISR_EOC) || (ADC2->ISR & ADC_ISR_EOS)) {
        ADC2->ISR &= ~(ADC_ISR_EOC | ADC_ISR_EOS);
        adcData = ADC2->DR;
//        pidOperate();
    }
}

void initPID() {

    System::Task pidThread("PID_Thread", 1024, System::RTOSPriority::Normal2);
    pidThread.createTask(pidTaskProcedure, nullptr);

}
