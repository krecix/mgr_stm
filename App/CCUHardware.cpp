#include "CCUHardware.hpp"

#include "stm32h7a3xxq.h"

#include "STM32H7A3_GPIO.hpp"
#include "STM32H7A3_UART.hpp"
#include "STM32H7A3_Positioner.hpp"
#include "STM32H7A3_Engine.hpp"
#include "STM32H7A3_ElectricCoils.hpp"
#include "STM32H7A3_ADC.hpp"

#include "VibrationController.hpp"

namespace ccu {

    void setupClocks(void);

    void initPositioners(void);

//Positioners

    Drivers::STM32H7A3_GPIO positioners[] = {
            Drivers::STM32H7A3_GPIO{GPIOD, Drivers::GPIO_PIN::Pin0, LOW},
            Drivers::STM32H7A3_GPIO{GPIOD, Drivers::GPIO_PIN::Pin1, LOW},
            Drivers::STM32H7A3_GPIO{GPIOD, Drivers::GPIO_PIN::Pin2, LOW},
            Drivers::STM32H7A3_GPIO{GPIOD, Drivers::GPIO_PIN::Pin3, LOW},
            Drivers::STM32H7A3_GPIO{GPIOD, Drivers::GPIO_PIN::Pin4, LOW},
            Drivers::STM32H7A3_GPIO{GPIOD, Drivers::GPIO_PIN::Pin5, LOW},
            Drivers::STM32H7A3_GPIO{GPIOD, Drivers::GPIO_PIN::Pin6, LOW},
            Drivers::STM32H7A3_GPIO{GPIOD, Drivers::GPIO_PIN::Pin7, LOW}
    };
    STM32H7A3_Positioner positioner(reinterpret_cast<volatile const uint8_t *>(&GPIOD->IDR));

//LimitTransoptors
    Drivers::STM32H7A3_GPIO limitA(GPIOE, Drivers::GPIO_PIN::Pin15, HIGH);
    Drivers::STM32H7A3_GPIO limitB(GPIOE, Drivers::GPIO_PIN::Pin14, HIGH);

    Limiters limiters
            {
                    limitA,
                    limitB
            };

//AbstractEngine

    Drivers::STM32H7A3_GPIO tim2Ch1_Pin(GPIOA, Drivers::GPIO_PIN::Pin0, HIGH);
    Drivers::STM32H7A3_GPIO engineLeft(GPIOA, Drivers::GPIO_PIN::Pin1, HIGH);
    Drivers::STM32H7A3_GPIO engineRight(GPIOA, Drivers::GPIO_PIN::Pin2, HIGH);
    STM32H7A3_Engine engine(engineLeft, engineRight);

//ElectroCoils

    Drivers::STM32H7A3_GPIO leftCoil(GPIOC, Drivers::GPIO_PIN::Pin6, HIGH);
    Drivers::STM32H7A3_GPIO rightCoil(GPIOE, Drivers::GPIO_PIN::Pin9, HIGH);
    Drivers::STM32H7A3_ElectricCoils electricCoils;

//Blink leds

    Drivers::STM32H7A3_GPIO greenLed(GPIOB, Drivers::GPIO_PIN::Pin0, LOW);
    Drivers::STM32H7A3_GPIO redLed(GPIOB, Drivers::GPIO_PIN::Pin14, LOW);
    Drivers::STM32H7A3_GPIO yellowLed(GPIOE, Drivers::GPIO_PIN::Pin1, LOW);

    LEDS leds = {
            .greenLed = greenLed,
            .redLed = redLed,
            .yellowLed = yellowLed
    };

//UART3 

    Drivers::STM32H7A3_GPIO uart3Tx(GPIOD, Drivers::GPIO_PIN::Pin8, HIGH);
    Drivers::STM32H7A3_GPIO uart3Rx(GPIOD, Drivers::GPIO_PIN::Pin9, HIGH);

    Drivers::STM32H7A3_UART uart3(USART3, 137500000UL, 921600UL,
                                  DMA2_Stream1, DMA2_Stream0);


//ADC2

    Drivers::STM32H7A3_GPIO adcPin(GPIOC, Drivers::GPIO_PIN::Pin3, LOW);
    Drivers::STM32H7A3_ADC adc2;


//VibrationController

    VibrationController vibrationController{
            limiters,
            positioner,
            engine,
            electricCoils,
            adc2
    };

    CCUHardware::CCUHardware()
            : Leds{leds},
              Terminal{uart3},
              VibrationControllerUnit{vibrationController} {
    }

    void startupProcedure();


    void CCUHardware::Initialize() {
        startupProcedure();
        initPositioners();

        //Leds
        greenLed.setMode(Drivers::GPIO_MODER::GeneralOutput);
        yellowLed.setMode(Drivers::GPIO_MODER::GeneralOutput);
        redLed.setMode(Drivers::GPIO_MODER::GeneralOutput);
        /////

        //LimitTransoptors
        limitA.setMode(Drivers::GPIO_MODER::Input);
        limitA.setSpeed(Drivers::GPIO_SPEEDR::VeryHighSpeed);
        limitA.setPull_Up_Down(Drivers::GPIO_PULL_UP_DOWN::No_PullUp_PullDown);
        limitB.setMode(Drivers::GPIO_MODER::Input);
        limitB.setSpeed(Drivers::GPIO_SPEEDR::VeryHighSpeed);
        limitB.setPull_Up_Down(Drivers::GPIO_PULL_UP_DOWN::No_PullUp_PullDown);
        /////

        //AbstractEngine
        engineLeft.setMode(Drivers::GPIO_MODER::GeneralOutput);
        engineLeft.setType(Drivers::GPIO_TYPER::OpenDrain);
        engineRight.setMode(Drivers::GPIO_MODER::GeneralOutput);
        engineRight.setType(Drivers::GPIO_TYPER::OpenDrain);
        tim2Ch1_Pin.setMode(Drivers::GPIO_MODER::AlternateFunction);
        tim2Ch1_Pin.setType(Drivers::GPIO_TYPER::OpenDrain);
        tim2Ch1_Pin.setAlternateFunction(Drivers::GPIO_ALTERNATE_FUNCTION::AF1);
        engine.initialize(40'000UL);
        /////

        //Electric Coils
        leftCoil.setMode(Drivers::GPIO_MODER::AlternateFunction);
        leftCoil.setType(Drivers::GPIO_TYPER::OpenDrain);
        leftCoil.setAlternateFunction(Drivers::GPIO_ALTERNATE_FUNCTION::AF3);
        rightCoil.setMode(Drivers::GPIO_MODER::AlternateFunction);
        rightCoil.setType(Drivers::GPIO_TYPER::OpenDrain);
        rightCoil.setAlternateFunction(Drivers::GPIO_ALTERNATE_FUNCTION::AF1);
        electricCoils.initialize(40'000UL);
        /////

        //UART3
        uart3Tx.setMode(Drivers::GPIO_MODER::AlternateFunction);
        uart3Tx.setAlternateFunction(Drivers::GPIO_ALTERNATE_FUNCTION::AF7);
        uart3Rx.setMode(Drivers::GPIO_MODER::AlternateFunction);
        uart3Rx.setAlternateFunction(Drivers::GPIO_ALTERNATE_FUNCTION::AF7);
        auto priority = NVIC_EncodePriority(0x00, 6, 0);
        NVIC_SetPriority(USART3_IRQn, priority);
        NVIC_EnableIRQ(USART3_IRQn);
        //RxDMA
        NVIC_SetPriority(DMA2_Stream1_IRQn, priority);
        NVIC_EnableIRQ(DMA2_Stream1_IRQn);
        // DMAMUX Request USART3 Rx
        DMAMUX1_Channel9->CCR = 45;
        DMAMUX1_ChannelStatus->CFR = DMAMUX_CFR_CSOF9; // I took this from HAL example
        //TxDMA
        NVIC_SetPriority(DMA2_Stream0_IRQn, priority);
        NVIC_EnableIRQ(DMA2_Stream0_IRQn);
        // DMAMUX Request USART3 Tx
        DMAMUX1_Channel8->CCR = 46;
        DMAMUX1_ChannelStatus->CFR = DMAMUX_CFR_CSOF8; // I took this from HAL example

        uart3.initialize();
        /////

        //ADC2
        NVIC_SetPriority(DMA1_Stream0_IRQn, priority);
        NVIC_EnableIRQ(DMA1_Stream0_IRQn);
        NVIC_SetPriority(DMA1_Stream1_IRQn, priority);
        NVIC_EnableIRQ(DMA1_Stream1_IRQn);
        adcPin.setMode(Drivers::GPIO_MODER::Analog);
        adc2.init();
        /////

    }

    void initPositioners() {
        for (auto &positionersElement : positioners) {
            positionersElement.setMode(Drivers::GPIO_MODER::Input);
            positionersElement.setSpeed(Drivers::GPIO_SPEEDR::VeryHighSpeed);
            positionersElement.setPull_Up_Down(Drivers::GPIO_PULL_UP_DOWN::No_PullUp_PullDown);
        }
    }


    void startupProcedure() {
        // Enable Instruction Cache L1
        SCB_InvalidateICache();
        SCB_EnableICache();

        PWR->CR3 &= ~(PWR_CR3_BYPASS | PWR_CR3_LDOEN);
        PWR->CR3 |= PWR_CR3_SMPSEN;

        while (!(PWR->CSR1 & PWR_CSR1_ACTVOSRDY));
        /** Configure the main internal regulator output voltage
       */
        PWR->SRDCR |= PWR_SRDCR_VOS;

        while (!(PWR->SRDCR & PWR_SRDCR_VOSRDY));

        setupClocks();
        SystemCoreClockUpdate();

        // Init AbstractGPIO clocks
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIOCEN | RCC_AHB4ENR_GPIODEN |
                        RCC_AHB4ENR_GPIOEEN;
        //USART3 clock
        RCC->APB1LENR |= RCC_APB1LENR_USART3EN;
        //DMA2 clock
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
        //USB clock source
        RCC->CDCCIP2R |= RCC_CDCCIP2R_USBSEL;
        //TIM1 clock
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
        //TIM2 clock
        RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
        //TIM3 clock
        RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;
        //TIM8 clock
        RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;

        //ADC2 clock
        RCC->CDCCIPR |= RCC_CDCCIPR_CKPERSEL_1;
        RCC->AHB1ENR |= RCC_AHB1ENR_ADC12EN;
        //DMA clock
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMA2EN;
    }

    void setupClocks() {
        // HSE STLink 8.333333MHz
        // Out: 275MHz

        RCC->CR |= RCC_CR_HSEON | RCC_CR_HSI48ON | RCC_CR_HSEBYP;
        while (!(RCC->CR & RCC_CR_HSERDY));
        // DIVM1 /1, DIVM2 /1, DIVM3 /1
        RCC->PLLCKSELR |=
                RCC_PLLCKSELR_DIVM1_0 | RCC_PLLCKSELR_DIVM2_0 | RCC_PLLCKSELR_DIVM3_0 | RCC_PLLCKSELR_PLLSRC_HSE;
        // PLL1
        // DIVN1 x66, DIVP1 /2, DIVQ1 /4, DIVR1 /2
        RCC->PLL1DIVR = (0x41UL << 0U) | (1UL << 9U) | (3UL << 16U) | (1UL << 24U);
        RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_3 | RCC_PLLCFGR_DIVP1EN | RCC_PLLCFGR_DIVQ1EN | RCC_PLLCFGR_DIVR1EN;
        RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL1VCOSEL;

        RCC->CDCFGR1 |= RCC_CDCFGR1_CDPPRE_2;
        RCC->CDCFGR2 |= RCC_CDCFGR2_CDPPRE1_DIV2 | RCC_CDCFGR2_CDPPRE2_DIV2;
        RCC->SRDCFGR |= RCC_SRDCFGR_SRDPPRE_DIV2;

        // PLL2
        // DIVN2 x66, DIVP2 /15, DIVQ2 /2, DIVR2 /2
        RCC->PLL2DIVR = (65UL << 0U) | (14UL << 9U) | (1UL << 16U) | (1UL << 24U);
        RCC->PLLCFGR |= RCC_PLLCFGR_PLL2RGE_3 | RCC_PLLCFGR_DIVP2EN | RCC_PLLCFGR_DIVQ2EN | RCC_PLLCFGR_DIVR2EN;
        RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL2VCOSEL;

        // PLL3
        // DIVN3 x43, DIVP3 /4, DIVQ3 /2, DIVR3 /2
        //	RCC->PLL3DIVR = (42UL << 0U) | (3UL << 9U) | (1UL << 16U) | (1UL <<
        //24U); DIVN3 x23, DIVP3 /2, DIVQ3 /4, DIVR3 /2
        RCC->PLL3DIVR = (22UL << 0U) | (1UL << 9U) | (3UL << 16U) | (1UL << 24U);
        RCC->PLLCFGR |= RCC_PLLCFGR_PLL3RGE_3 | RCC_PLLCFGR_DIVP3EN | RCC_PLLCFGR_DIVQ3EN | RCC_PLLCFGR_DIVR3EN;
        RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL3VCOSEL;

        RCC->CR |= RCC_CR_PLL1ON | RCC_CR_PLL2ON | RCC_CR_PLL3ON;

        FLASH->ACR = FLASH_ACR_LATENCY_7WS | (3UL << 4);
        while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_7WS);

        while (!(RCC->CR & RCC_CR_PLL1RDY));
        while (!(RCC->CR & RCC_CR_PLL2RDY));
        while (!(RCC->CR & RCC_CR_PLL3RDY));
        RCC->CFGR |= RCC_CFGR_SW_PLL1;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL1);
        RCC->CR &= ~RCC_CR_HSION;
    }


} /* namespace ccu */

extern "C" [[maybe_unused]] void USART3_IRQHandler(void) {
    ccu::uart3.Interrupt();
}

extern "C" void DMA2_Stream0_IRQHandler(void) {
    if (DMA2->LISR & DMA_LISR_TCIF0) {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0;
        ccu::uart3.txDMAInterrupt_TransferComplete();
    }
}

extern "C" void DMA2_Stream1_IRQHandler(void) {
    if (DMA2->LISR & DMA_LISR_TCIF1) {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF1;
        ccu::uart3.rxDMAInterrupt_TransferComplete();
    }
}

//Actual value circular mode
extern "C" void DMA1_Stream0_IRQHandler(void) {
    if (DMA1->LISR & DMA_LISR_TCIF0) {
        DMA1->LIFCR |= DMA_LIFCR_CTCIF0;
        ccu::adc2.DMA1_Stream0_Interrupt();
    }
}
//Buffer mode
extern "C" void DMA1_Stream1_IRQHandler(void) {
    if (DMA1->LISR & DMA_LISR_TCIF1) {
        DMA1->LIFCR |= DMA_LIFCR_CTCIF1;
        ccu::adc2.DMA1_Stream1_Interrupt();
    }
}

extern "C" [[maybe_unused]] void Error_Handler(void) {
    asm("bkpt");
}