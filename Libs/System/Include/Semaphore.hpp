#ifndef __Semaphore_HPP__
#define __Semaphore_HPP__

#include "cmsis_os2.h"

#include "stdint.h"
#include "stddef.h"

#include "RTOSDefines.hpp"

namespace System {

    class Semaphore {
    public:
        Semaphore(const char *name);

        bool createSemaphore(uint32_t maxCount, uint32_t initCount);

        RTOSStatus release(void);

        RTOSStatus get(uint32_t timeout);

    private:
        osSemaphoreAttr_t _attr;
        osSemaphoreId_t _id;
    };


}

#endif /* __Semaphore_HPP__ */