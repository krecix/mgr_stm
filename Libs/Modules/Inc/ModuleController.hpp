//
// Created by krzysztof on 03.05.2021.
//

#ifndef VIBRATIONCONTROLSTM32_MODULECONTROLLER_HPP
#define VIBRATIONCONTROLSTM32_MODULECONTROLLER_HPP

#include "Config.hpp"
#include "Pattern.hpp"
#include "VibrationController.hpp"
#include "Task.hpp"
#include "PIDController.hpp"

enum class ModuleControllerStatus_t {
    Ok,
    InvalidPattern,
    InUse
};

//Main Module Controller
class ModuleController {
public:
    ModuleController(VibrationController &vibrationController, Pattern &pattern);

    ModuleControllerStatus_t checkPattern(etl::delegate<void(uint16_t setpoint, uint16_t *measurement, size_t length)> callback);

    Pattern& getPattern() {
        return m_pattern;
    }

    ModuleControllerStatus_t checkPID(float P, float I, float D,
                                      etl::delegate<void(uint16_t setpoint, uint16_t *measurement, size_t length)> callback);

private:
    [[noreturn]] void checkPatternProcedure(void *args);
    [[noreturn]] void checkPIDProcedure(void *args);
    [[noreturn]] void checkVibrationReduceProcedure(void *args);

    bool busy;
    VibrationController &m_vibrationControllerUnit;
    Pattern &m_pattern;

    System::Task checkPatternTask;
    System::Task checkPIDTask;
    int32_t setInitialStateADC(); // Stabilize adc measurements and returns setpoint
    Position convertPosition(uint8_t position);
    void goToPosition(Position position);

    uint16_t adcBuffer[COMMUNICATOR_BUFFER_MAX_SIZE];
    PIDController<float, int32_t>::PIDParameters pidParameters {
        .Kp = 0.0,
        .Ki = 0.0,
        .Kd = 0.0,
        .antiWindupLimit = 1000
    };
    PIDController<float, int32_t> pidController{pidParameters};
};

#endif //VIBRATIONCONTROLSTM32_MODULECONTROLLER_HPP
