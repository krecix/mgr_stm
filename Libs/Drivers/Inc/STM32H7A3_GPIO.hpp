#ifndef __STM32H7A3_GPIO_HPP__
#define __STM32H7A3_GPIO_HPP__

#include "AbstractGPIO.hpp"

#include "stm32h7a3xxq.h"

namespace Drivers {


    enum class GPIO_PIN {
        Pin0,
        Pin1,
        Pin2,
        Pin3,
        Pin4,
        Pin5,
        Pin6,
        Pin7,
        Pin8,
        Pin9,
        Pin10,
        Pin11,
        Pin12,
        Pin13,
        Pin14,
        Pin15
    };

    enum class GPIO_MODER {
        Input,
        GeneralOutput,
        AlternateFunction,
        Analog
    };

    enum class GPIO_TYPER {
        PushPull,
        OpenDrain
    };

    enum class GPIO_SPEEDR {
        LowSpeed,
        MediumSpeed,
        HighSpeed,
        VeryHighSpeed
    };

    enum class GPIO_PULL_UP_DOWN {
        No_PullUp_PullDown,
        PullUp,
        PullDown
    };

    enum class GPIO_ALTERNATE_FUNCTION {
        AF0,
        AF1,
        AF2,
        AF3,
        AF4,
        AF5,
        AF6,
        AF7,
        AF8,
        AF9,
        AF10,
        AF11,
        AF12,
        AF13,
        AF14,
        AF15
    };

    using GPIO_Pin_t = uint32_t;


    class STM32H7A3_GPIO final : public AbstractGPIO {
    public:
        explicit STM32H7A3_GPIO(GPIO_TypeDef *port, GPIO_PIN pin, bool initState);

        virtual ~STM32H7A3_GPIO() {}

        inline void High(void) {
            portPtr->BSRR = (1UL << gpioPin);
        }

        inline void Low(void) {
            portPtr->BSRR = (1UL << (gpioPin + 16UL));
        }

        inline void Toggle(void) {
            portPtr->ODR ^= (1UL << gpioPin);
        }

        inline bool getState(void) {
            return (portPtr->IDR & (1UL << gpioPin));
        }

        void setMode(GPIO_MODER mode);

        void setType(GPIO_TYPER type);

        void setSpeed(GPIO_SPEEDR speed);

        void setPull_Up_Down(GPIO_PULL_UP_DOWN pull);

        void setAlternateFunction(GPIO_ALTERNATE_FUNCTION function);

    private:

        GPIO_TypeDef *portPtr;
        GPIO_Pin_t gpioPin;
        bool initState;

        uint32_t afrNumberRegister;

    };

}

#endif /* __STM32H7A3_GPIO_HPP__ */