#ifndef __CCUHardware_HPP__
#define __CCUHardware_HPP__

#include "AbstractGPIO.hpp"
#include "AbstractUART.hpp"
#include "AbstractPositioner.hpp"
#include "AbstractEngine.hpp"
#include "AbstractElectricCoils.hpp"
#include "AbstractADC.hpp"

#include "VibrationController.hpp"

namespace ccu {

//Green LED PB0
//Red LED PB14
//Yellow LED PE1
    struct LEDS {
        AbstractGPIO &greenLed;
        AbstractGPIO &redLed;
        AbstractGPIO &yellowLed;
    };


    struct UARTPins {
        AbstractGPIO &Tx;
        AbstractGPIO &Rx;
    };

    struct CCUHardware final {
        CCUHardware();

        void Initialize();

        LEDS &Leds;
        AbstractUART &Terminal;
        VibrationController &VibrationControllerUnit;
    };

} /* namespace ccu */

#endif /* __CCUHardware_HPP__ */
