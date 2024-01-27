#ifndef __TASK_HPP__
#define __TASK_HPP__

#include "cmsis_os2.h"

#include "stdint.h"
#include "stddef.h"

#include "RTOSDefines.hpp"

namespace System {

    typedef void (*ThreadProcedure)(void *arguments);

    class Task {
    public:
        Task(const char *name, size_t stackSize, RTOSPriority priority);

        bool createTask(ThreadProcedure procedure, void *arguments);

        RTOSStatus suspendTask();

        RTOSStatus resumeTask();

        [[nodiscard]] auto isCreated() const {
            return created;
        }

    private:
        Task(void) = delete;

        osThreadAttr_t _attr;
        osThreadId_t _threadId;
        bool created;
    };

}

#endif /* __TASK_HPP__ */