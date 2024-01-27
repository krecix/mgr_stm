//
// Created by krzysztof on 03.05.2021.
//

#include "VibrationController.hpp"


VibrationController::VibrationController(Limiters &limiters, AbstractPositioner &positioner,
                                         AbstractEngine &engine, AbstractElectricCoils &electricCoils,
                                         AbstractADC &adc)
        : LimitersUnit{limiters},
          PositionerUnit{positioner},
          EngineUnit{engine},
          ElectricCoilsUnit{electricCoils},
          AdcUnit{adc},
          lastPosition{Position::None} {

}

void VibrationController::InitializeProcedure() {
    goToInitialPosition();
}

void VibrationController::goToInitialPosition() {
    while (goToPosition(Position::First) != OnPosition::True);
}

VibrationController::OnPosition VibrationController::goToPosition(Position position) {
    if(position == Position::None) {
        return OnPosition::True;
    }
    auto actualPosition = PositionerUnit.getPosition();
    if (actualPosition == position) {
        EngineUnit.stop();
        lastPosition = actualPosition;
        return OnPosition::True;
    }
    if (LimitersUnit.LimitTransoptorA.getState()) {
        lastPosition = Position::LeftTransoptor;
        EngineUnit.goRight();
        return OnPosition::False;
    }
    if (LimitersUnit.LimitTransoptorB.getState()) {
        lastPosition = Position::RightTransoptor;
        EngineUnit.goLeft();
        return OnPosition::False;
    }
    if (lastPosition == Position::LeftTransoptor) {
        EngineUnit.goRight();
    } else {
        if ((lastPosition < position) && (lastPosition != Position::None)) {
            EngineUnit.goRight();
        } else {
            EngineUnit.goLeft();
        }
    }
    if (actualPosition != Position::None) {
        lastPosition = actualPosition;
    }
    return OnPosition::False;
}

void VibrationController::goRight()
{
    EngineUnit.goRight();
}

void VibrationController::goLeft()
{
    EngineUnit.goLeft();
}

void VibrationController::stop() {
    EngineUnit.stop();
    lastPosition = PositionerUnit.getPosition();
}

