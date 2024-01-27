#ifndef __SOFTWARETIMER_HPP__
#define __SOFTWARETIMER_HPP__

#include "cmsis_os2.h"

#include "stdint.h"
#include "stddef.h"

#include "RTOSDefines.hpp"

namespace System {

    typedef void (*SoftTimerFunction)(void *arguments);

    class SoftwareTimer {
    public:
        SoftwareTimer() = default;

        bool create(SoftTimerFunction timerFunction, SoftTimerType type, void *arguments);

        bool start(uint32_t periodTicks);

        bool stop(void);

    private:
        osTimerId_t _id;
    };

}

#endif /* __SOFTWARETIMER_HPP__ */