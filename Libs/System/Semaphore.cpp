#include "Semaphore.hpp"


namespace System {

    Semaphore::Semaphore(const char *name) {
        _attr = {
                .name = name,
                .attr_bits = 0,
                .cb_mem = nullptr,
                .cb_size = 0
        };
    }

    bool Semaphore::createSemaphore(uint32_t maxCount, uint32_t initCount) {
        _id = osSemaphoreNew(maxCount, initCount, &_attr);

        if (_id == 0)
            return false;

        return true;
    }

    RTOSStatus Semaphore::release(void) {
        return static_cast<RTOSStatus>( osSemaphoreRelease(_id));
    }

    RTOSStatus Semaphore::get(uint32_t timeout) {
        return static_cast<RTOSStatus>( osSemaphoreAcquire(_id, timeout));
    }

}