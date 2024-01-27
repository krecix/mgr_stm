#include "Task.hpp"


namespace System {

    Task::Task(const char *name, size_t stackSize, RTOSPriority priority) : created{false} {
        _attr =
                {
                        .name = name,
                        .attr_bits = osThreadDetached,
                        .cb_mem = nullptr,
                        .cb_size = 0,
                        .stack_mem = nullptr,
                        .stack_size = stackSize,
                        .priority = static_cast<osPriority_t>(priority),
                        .tz_module = 0,
                        .reserved = 0
                };
    }

    bool Task::createTask(ThreadProcedure procedure, void *arguments) {
        _threadId = osThreadNew(procedure, arguments, &_attr);

        if (_threadId == 0)
            return false;

        created = true;
        return true;
    }

    RTOSStatus Task::suspendTask(void) {
        return static_cast<RTOSStatus>(osThreadSuspend(_threadId));
    }

    RTOSStatus Task::resumeTask(void) {
        return static_cast<RTOSStatus>(osThreadResume(_threadId));
    }


}