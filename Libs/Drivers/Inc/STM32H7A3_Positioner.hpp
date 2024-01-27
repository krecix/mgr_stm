#ifndef __STM32H7A3_Positioner_HPP__
#define __STM32H7A3_Positioner_HPP__

#include "AbstractPositioner.hpp"

class STM32H7A3_Positioner final : public AbstractPositioner {
public:
    explicit STM32H7A3_Positioner(volatile const uint8_t *positionerInputAddress);

    inline Position getPosition() const override {
        return Position{static_cast<Position>(_position)};
    }

private:
    STM32H7A3_Positioner() = delete;

    volatile const uint8_t &_position;
};

#endif //__STM32H7A3_Positioner_HPP__