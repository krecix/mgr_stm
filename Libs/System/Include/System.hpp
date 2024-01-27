#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include "Task.hpp"
#include "SoftwareTimer.hpp"
#include "Semaphore.hpp"

namespace System {
    inline uint32_t TicksFromMs(uint32_t milliseconds) {
        return milliseconds / (1000UL / osKernelGetTickFreq());
    }

    inline void KernelInit(void) {
        osKernelInitialize();
    }

    inline void KernelStart(void) {
        osKernelStart();
    }

}


#endif /* __SYSTEM_HPP__ */