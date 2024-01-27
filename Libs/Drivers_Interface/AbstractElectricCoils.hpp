//
// Created by krzysztof on 27.03.2021.
//

#ifndef VIBRATIONCONTROLSTM32_ABSTRACTELECTRICCOILS_HPP
#define VIBRATIONCONTROLSTM32_ABSTRACTELECTRICCOILS_HPP

#include <stdint.h>

//AbstractElectricCoils interface
struct AbstractElectricCoils {
    virtual inline void stop() = 0;

    virtual inline void left(uint32_t duty) = 0;

    virtual inline void right(uint32_t duty) = 0;
};


#endif //VIBRATIONCONTROLSTM32_ABSTRACTELECTRICCOILS_HPP
