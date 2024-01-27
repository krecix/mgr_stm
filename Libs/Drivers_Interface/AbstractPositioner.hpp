#ifndef __Positioner_HPP__
#define __Positioner_HPP__

#include <stdint.h>

enum class Position : uint8_t {
    None = 0,
    First = 1,
    Second = 2,
    Third = 4,
    Fourth = 8,
    Fifth = 16,
    Sixth = 32,
    Seventh = 64,
    Eighth = 128,
    LeftTransoptor,
    RightTransoptor
};

//AbstractPositioner interface
struct AbstractPositioner {
    [[nodiscard]] inline virtual Position getPosition() const = 0;
};

#endif //__Positioner_HPP__