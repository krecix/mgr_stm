#ifndef __Engine_HPP__
#define __Engine_HPP__

#include <stdint.h>

//AbstractEngine interface
struct AbstractEngine {
    virtual inline void stop() = 0;

    virtual inline void goLeft() = 0;

    virtual inline void goRight() = 0;
};

#endif //__Engine_HPP__ 