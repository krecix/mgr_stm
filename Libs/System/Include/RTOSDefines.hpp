#ifndef __RTOSDEFINES_HPP__
#define __RTOSDEFINES_HPP__

namespace System {


/// Status code values
    enum class RTOSStatus {
        OK = 0,         ///< Operation completed successfully.
        Error = -1,         ///< Unspecified RTOS error: run-time error but no other error message fits.
        ErrorTimeout = -2,         ///< Operation not completed within the timeout period.
        ErrorResource = -3,         ///< Resource not available.
        ErrorParameter = -4,         ///< Parameter error.
        ErrorNoMemory = -5,         ///< System is out of memory: it was impossible to allocate or reserve memory for the operation.
        ErrorISR = -6,         ///< Not allowed in ISR context: the function cannot be called from interrupt service routines.
        StatusReserved = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
    };


    enum class RTOSPriority {
        None = 0,         ///< No priority (not initialized).
        Idle = 1,         ///< Reserved for Idle thread.
        Low = 8,         ///< Priority: low
        Low1 = 8 + 1,       ///< Priority: low + 1
        Low2 = 8 + 2,       ///< Priority: low + 2
        Low3 = 8 + 3,       ///< Priority: low + 3
        Low4 = 8 + 4,       ///< Priority: low + 4
        Low5 = 8 + 5,       ///< Priority: low + 5
        Low6 = 8 + 6,       ///< Priority: low + 6
        Low7 = 8 + 7,       ///< Priority: low + 7
        BelowNormal = 16,         ///< Priority: below normal
        BelowNormal1 = 16 + 1,       ///< Priority: below normal + 1
        BelowNormal2 = 16 + 2,       ///< Priority: below normal + 2
        BelowNormal3 = 16 + 3,       ///< Priority: below normal + 3
        BelowNormal4 = 16 + 4,       ///< Priority: below normal + 4
        BelowNormal5 = 16 + 5,       ///< Priority: below normal + 5
        BelowNormal6 = 16 + 6,       ///< Priority: below normal + 6
        BelowNormal7 = 16 + 7,       ///< Priority: below normal + 7
        Normal = 24,         ///< Priority: normal
        Normal1 = 24 + 1,       ///< Priority: normal + 1
        Normal2 = 24 + 2,       ///< Priority: normal + 2
        Normal3 = 24 + 3,       ///< Priority: normal + 3
        Normal4 = 24 + 4,       ///< Priority: normal + 4
        Normal5 = 24 + 5,       ///< Priority: normal + 5
        Normal6 = 24 + 6,       ///< Priority: normal + 6
        Normal7 = 24 + 7,       ///< Priority: normal + 7
        AboveNormal = 32,         ///< Priority: above normal
        AboveNormal1 = 32 + 1,       ///< Priority: above normal + 1
        AboveNormal2 = 32 + 2,       ///< Priority: above normal + 2
        AboveNormal3 = 32 + 3,       ///< Priority: above normal + 3
        AboveNormal4 = 32 + 4,       ///< Priority: above normal + 4
        AboveNormal5 = 32 + 5,       ///< Priority: above normal + 5
        AboveNormal6 = 32 + 6,       ///< Priority: above normal + 6
        AboveNormal7 = 32 + 7,       ///< Priority: above normal + 7
        High = 40,         ///< Priority: high
        High1 = 40 + 1,       ///< Priority: high + 1
        High2 = 40 + 2,       ///< Priority: high + 2
        High3 = 40 + 3,       ///< Priority: high + 3
        High4 = 40 + 4,       ///< Priority: high + 4
        High5 = 40 + 5,       ///< Priority: high + 5
        High6 = 40 + 6,       ///< Priority: high + 6
        High7 = 40 + 7,       ///< Priority: high + 7
        Realtime = 48,         ///< Priority: realtime
        Realtime1 = 48 + 1,       ///< Priority: realtime + 1
        Realtime2 = 48 + 2,       ///< Priority: realtime + 2
        Realtime3 = 48 + 3,       ///< Priority: realtime + 3
        Realtime4 = 48 + 4,       ///< Priority: realtime + 4
        Realtime5 = 48 + 5,       ///< Priority: realtime + 5
        Realtime6 = 48 + 6,       ///< Priority: realtime + 6
        Realtime7 = 48 + 7,       ///< Priority: realtime + 7
        PriorityISR = 56,         ///< Reserved for ISR deferred thread.
        PriorityError = -1,         ///< System cannot determine priority or illegal priority.
        PriorityReserved = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
    };

    enum class SoftTimerType {
        OneShot = 0,
        Periodic
    };

    constexpr uint32_t WaitForever = osWaitForever;


}

#endif /* __RTOSDEFINES_HPP__ */ 