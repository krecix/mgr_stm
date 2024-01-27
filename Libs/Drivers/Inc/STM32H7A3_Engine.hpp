#ifndef __STM32H7A3_Engine_HPP__
#define __STM32H7A3_Engine_HPP__

#include "AbstractEngine.hpp"
#include "AbstractGPIO.hpp"


//AbstractEngine driver working on TIM2 Channel 1
class STM32H7A3_Engine final : public AbstractEngine {
public:
    explicit STM32H7A3_Engine(AbstractGPIO &dirLeft_Pin, AbstractGPIO &dirRight_Pin);

    void initialize(uint32_t frequency);

    inline void stop() override {
        _dirLeft.High();
        _dirRight.High();
    }

    inline void goLeft() override {
        _dirRight.High();
        _dirLeft.Low();
    }

    inline void goRight() override {
        _dirLeft.High();
        _dirRight.Low();
    }

private:
    STM32H7A3_Engine() = delete;

    AbstractGPIO &_dirLeft;
    AbstractGPIO &_dirRight;
};


#endif //__STM32H7A3_Engine_HPP__