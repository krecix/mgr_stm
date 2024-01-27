#include "STM32H7A3_GPIO.hpp"

namespace Drivers {


    STM32H7A3_GPIO::STM32H7A3_GPIO(GPIO_TypeDef *port, GPIO_PIN pin, bool initState) {

        this->portPtr = port;
        this->gpioPin = static_cast<uint32_t>(pin);
        this->initState = initState;

        if (initState == HIGH) {
            portPtr->ODR |= (1UL << gpioPin);
        } else {
            portPtr->ODR &= ~(1UL << gpioPin);
        }

        if (this->gpioPin < 8UL)
            afrNumberRegister = 0UL;
        else
            afrNumberRegister = 1UL;


    }

    void STM32H7A3_GPIO::setMode(GPIO_MODER mode) {
        uint32_t mode_cast = static_cast<uint32_t>(mode);
        portPtr->MODER = (portPtr->MODER & (~(3UL << (gpioPin * 2UL)))) | (mode_cast << (gpioPin * 2UL));
    }

    void STM32H7A3_GPIO::setType(GPIO_TYPER type) {
        uint32_t type_cast = static_cast<uint32_t>(type);
        portPtr->OTYPER = (portPtr->OTYPER & (~(1UL << (gpioPin)))) | (type_cast << (gpioPin));
    }

    void STM32H7A3_GPIO::setSpeed(GPIO_SPEEDR speed) {
        uint32_t speed_cast = static_cast<uint32_t>(speed);
        portPtr->OSPEEDR = (portPtr->OSPEEDR & (~(3UL << (gpioPin * 2UL)))) | (speed_cast << (gpioPin * 2UL));
    }

    void STM32H7A3_GPIO::setPull_Up_Down(GPIO_PULL_UP_DOWN pull) {
        uint32_t pull_cast = static_cast<uint32_t>(pull);
        portPtr->PUPDR = (portPtr->PUPDR & (~(3UL << (gpioPin * 2UL)))) | (pull_cast << (gpioPin * 2UL));
    }


    void STM32H7A3_GPIO::setAlternateFunction(GPIO_ALTERNATE_FUNCTION function) {
        uint32_t pinPosition;
        if (afrNumberRegister == 0)
            pinPosition = gpioPin;
        else
            pinPosition = gpioPin - 8;

        uint32_t function_cast = static_cast<uint32_t>(function);
        portPtr->AFR[afrNumberRegister] = (portPtr->AFR[afrNumberRegister] & (~(15UL << (pinPosition * 4UL)))) |
                                          (function_cast << (pinPosition * 4UL));
    }

}