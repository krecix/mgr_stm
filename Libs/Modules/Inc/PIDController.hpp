//
// Created by krzysztof on 06.04.2021.
//

#ifndef VIBRATIONCONTROLSTM32_PIDCONTROLLER_HPP
#define VIBRATIONCONTROLSTM32_PIDCONTROLLER_HPP

#include <cstdint>

template <typename PIDType, typename InputDataType>
class PIDController {
public:

    struct PIDParameters {
        PIDType Kp, Ki, Kd; // Member parameters
        InputDataType antiWindupLimit; // Anti-Windup integral limit
    };

    explicit PIDController(PIDParameters pid_parameters)
        : pidParameters{pid_parameters} {

    }

    auto getP() const {
        return pidParameters.Kp;
    }

    auto getI() const {

        return pidParameters.Ki;
    }

    auto getD() const {

        return pidParameters.Kd;
    }

    auto reset() {
        previousError = 0;
        totalError = 0;
    }

    auto setParameters(PIDParameters pid_parameters) {
        pidParameters = pid_parameters;
    }

    InputDataType calculate(InputDataType setpoint, InputDataType inputValue) {
        InputDataType error;

        PIDType p_term, i_term, d_term;

        error = setpoint - inputValue; //Calculate e(t)
        totalError += error; //Sum e(t)

        p_term = static_cast<PIDType>(pidParameters.Kp * error); //Response P member

        i_term = static_cast<PIDType>(pidParameters.Ki * totalError); // Response I member

        d_term = static_cast<PIDType>(pidParameters.Kd * (error - previousError)); // Response D member


        if (i_term >= pidParameters.antiWindupLimit) {
            i_term = pidParameters.antiWindupLimit;
        } else if (i_term <= -pidParameters.antiWindupLimit) {
            i_term = -pidParameters.antiWindupLimit;
        }

        previousError = error; //Update previous value variable

        return static_cast<InputDataType>(p_term + i_term + d_term);
    }

private:

    PIDParameters pidParameters;

    InputDataType previousError = 0; // Previous derivative error
    InputDataType totalError = 0; // Total integral error
};


#endif //VIBRATIONCONTROLSTM32_PIDCONTROLLER_HPP
