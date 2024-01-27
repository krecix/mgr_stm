#include "STM32H7A3_Positioner.hpp"


STM32H7A3_Positioner::STM32H7A3_Positioner(volatile const uint8_t *positionerInputAddress) :
        _position(*positionerInputAddress) {

}