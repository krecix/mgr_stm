//
// Created by krzysztof on 03.05.2021.
//

#ifndef VIBRATIONCONTROLSTM32_VIBRATIONCONTROLLER_HPP
#define VIBRATIONCONTROLSTM32_VIBRATIONCONTROLLER_HPP


#include "AbstractADC.hpp"
#include "AbstractEngine.hpp"
#include "AbstractPositioner.hpp"
#include "AbstractElectricCoils.hpp"
#include "AbstractGPIO.hpp"

struct Limiters {
    AbstractGPIO &LimitTransoptorA;
    AbstractGPIO &LimitTransoptorB;
};

class VibrationController {
public:

    enum class OnPosition {
        True,
        False
    };

    VibrationController(Limiters &limiters, AbstractPositioner &positioner,
                        AbstractEngine &engine, AbstractElectricCoils &electricCoils,
                        AbstractADC &adc);

    void InitializeProcedure();

    void goToInitialPosition();

    void goRight();
    void goLeft();

    OnPosition goToPosition(Position position);

    void stop();

    Limiters &LimitersUnit;
    AbstractPositioner &PositionerUnit;
    AbstractEngine &EngineUnit;
    AbstractElectricCoils &ElectricCoilsUnit;
    AbstractADC &AdcUnit;

private:
    Position lastPosition;
};


#endif //VIBRATIONCONTROLSTM32_VIBRATIONCONTROLLER_HPP
