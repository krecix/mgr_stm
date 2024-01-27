#include "SoftwareTimer.hpp"


namespace System {

    bool SoftwareTimer::create(SoftTimerFunction timerFunction, SoftTimerType type, void *arguments) {
        _id = osTimerNew(timerFunction, static_cast<osTimerType_t>(type), arguments, nullptr);
        if (_id == nullptr)
            return false;
        return true;
    }

    bool SoftwareTimer::start(uint32_t periodTicks) {
        auto status = osTimerStart(_id, periodTicks);
        if (status != osStatus_t::osOK)
            return false;
        return true;
    }

    bool SoftwareTimer::stop() {
        auto status = osTimerStop(_id);
        if (status != osStatus_t::osOK)
            return false;
        return true;
    }

}