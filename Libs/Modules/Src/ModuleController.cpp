//
// Created by krzysztof on 03.05.2021.
//

#include "ModuleController.hpp"
#include "System.hpp"

ModuleController::ModuleController(VibrationController &vibrationController,
                                   Pattern &pattern)
    : busy{false}, m_vibrationControllerUnit{vibrationController},
      m_pattern{pattern}, checkPatternTask{"CheckPattern_Task", 4096,
                                           System::RTOSPriority::High},
      checkPIDTask{"CheckPID_Task", 4096, System::RTOSPriority::High} {}

ModuleControllerStatus_t ModuleController::checkPattern(
    etl::delegate<void(uint16_t, uint16_t *, uint16_t *, size_t)> callback) {

  if (m_pattern.locked()) {
    return ModuleControllerStatus_t::InUse;
  } else if (m_pattern.empty()) {
    return ModuleControllerStatus_t::InvalidPattern;
  }

  struct Arguments {
    ModuleController *modulePtr;
    void *args;
  } arguments;

  arguments.modulePtr = this;
  arguments.args = &callback;

  if (!checkPatternTask.isCreated()) {
    checkPatternTask.createTask(
        [](void *args) {
          auto arguments = reinterpret_cast<Arguments *>(args);
          arguments->modulePtr->checkPatternProcedure(arguments->args);
        },
        &arguments);
  } else {
    checkPatternTask.resumeTask();
  }

  return ModuleControllerStatus_t::Ok;
}

void ModuleController::checkPatternProcedure(void *args) {
  etl::delegate<void(uint16_t, uint16_t *, uint16_t *, size_t)> callback =
      *reinterpret_cast<etl::delegate<void(uint16_t, uint16_t *, uint16_t *, size_t)> *>(
          args);
  bool started = false;
  bool finished = false;
  uint16_t actualValue;
  size_t receivedValueCount = 0;

  ADCCallbackConfig_t config{false, true}; // Actual value off, buffer on

  decltype(m_pattern.begin()) patternIterator;

  auto executePatternElement = [=](const auto &patternElement) {
    auto command = static_cast<PatternCommandType>(patternElement.command);
    switch (command) {
    case PatternCommandType::Go:
      goToPosition(convertPosition(patternElement.content.go.position));
      break;
    case PatternCommandType::Wait:
      osDelay(System::TicksFromMs(patternElement.content.wait.time));
      break;
    }
  };

  int32_t setpoint = 0;

  while (true) {

    if (!started) {
      m_pattern.lock();
      started = true;
      patternIterator = m_pattern.begin();

      m_vibrationControllerUnit.goToInitialPosition();
      setpoint = setInitialStateADC();
      m_vibrationControllerUnit.AdcUnit.start(
          adcBuffer, COMMUNICATOR_BUFFER_MAX_SIZE, &actualValue, config,
          [&]([[maybe_unused]] ReceiveADCValueType_t type, size_t length) {
            receivedValueCount = length;
            finished = true;
          });
    }

    if (finished) {
      finished = false;
      started = false;
      m_vibrationControllerUnit.goToInitialPosition();
      m_pattern.unlock();
      callback(setpoint, adcBuffer, pidResponseBuffer, receivedValueCount);
      checkPatternTask.suspendTask();
    }

    if (started) {
      if (patternIterator != m_pattern.end()) {
        const auto &element = *patternIterator;
        executePatternElement(element);
        patternIterator++;
      } else {
        m_vibrationControllerUnit.AdcUnit.stop();
      }
    }
  }
}

ModuleControllerStatus_t ModuleController::checkPID(
    float P, float I, float D,
    etl::delegate<void(uint16_t, uint16_t *, uint16_t *, size_t)> callback) {
  if (m_pattern.locked()) {
    return ModuleControllerStatus_t::InUse;
  } else if (m_pattern.empty()) {
    return ModuleControllerStatus_t::InvalidPattern;
  }

  PIDController<float, int32_t>::PIDParameters parameters{
      .Kp = P, .Ki = I, .Kd = D, .antiWindupLimit = 1000};

  pidController.setParameters(parameters);
  pidController.reset();

  struct Arguments {
    ModuleController *modulePtr;
    void *args;
  } arguments;

  arguments.modulePtr = this;
  arguments.args = &callback;

  if (!checkPIDTask.isCreated()) {
    checkPIDTask.createTask(
        [](void *args) {
          auto arguments = reinterpret_cast<Arguments *>(args);
          arguments->modulePtr->checkPIDProcedure(arguments->args);
        },
        &arguments);
  } else {
    checkPIDTask.resumeTask();
  }

  return ModuleControllerStatus_t::Ok;
}

static int32_t pidResponse = 0;
void ModuleController::checkPIDProcedure(void *args) {
  etl::delegate<void(uint16_t, uint16_t *, uint16_t *, size_t)> callback =
      *reinterpret_cast<etl::delegate<void(uint16_t, uint16_t *, uint16_t *, size_t)> *>(
          args);
  bool started = false;
  bool finished = false;
  uint16_t actualValue;
  size_t receivedValueCount = 0;

  ADCCallbackConfig_t config{true, true}; // Actual value on, buffer on

  decltype(m_pattern.begin()) patternIterator;

  auto executePatternElement = [=](const auto &patternElement) {
    auto command = static_cast<PatternCommandType>(patternElement.command);
    switch (command) {
    case PatternCommandType::Go:
      goToPosition(convertPosition(patternElement.content.go.position));
      break;
    case PatternCommandType::Wait:
      osDelay(System::TicksFromMs(patternElement.content.wait.time));
      break;
    }
  };

  int32_t PID_setpoint = 0;

  while (true) {
    if (!started) {
      m_pattern.lock();
      started = true;
      patternIterator = m_pattern.begin();

      m_vibrationControllerUnit.goToInitialPosition();
      PID_setpoint = setInitialStateADC();
      m_vibrationControllerUnit.AdcUnit.start(
          adcBuffer, COMMUNICATOR_BUFFER_MAX_SIZE, &actualValue, config,
          [&](ReceiveADCValueType_t type, size_t length) {
            if (type == ReceiveADCValueType_t::Actual) {
              test_adcBuffer[pidResponseNumber] = actualValue;
              pidResponse = pidController.calculate(PID_setpoint, actualValue);
              if (pidResponse < 0) {
                if (pidResponse < -1000) {
                  pidResponse = -1000;
                }
                m_vibrationControllerUnit.ElectricCoilsUnit.right(-pidResponse);
                m_vibrationControllerUnit.ElectricCoilsUnit.left(0);
              } else {
                if (pidResponse > 1000) {
                  pidResponse = 1000;
                }
                m_vibrationControllerUnit.ElectricCoilsUnit.left(pidResponse);
                m_vibrationControllerUnit.ElectricCoilsUnit.right(0);
              }
              pidResponseBuffer[pidResponseNumber] = pidResponse;
              pidResponseNumber++;
            } else {
              receivedValueCount = length;
              finished = true;
              pidResponseNumber = 0;
            }
          });
    }

    if (finished) {
      finished = false;
      started = false;
      m_vibrationControllerUnit.ElectricCoilsUnit.stop();
      m_vibrationControllerUnit.goToInitialPosition();
      m_pattern.unlock();
      callback(PID_setpoint, test_adcBuffer, pidResponseBuffer, receivedValueCount);
      checkPIDTask.suspendTask();
    }

    if (started) {
      if (patternIterator != m_pattern.end()) {
        const auto &element = *patternIterator;
        executePatternElement(element);
        patternIterator++;
      } else {
        if (pidResponseNumber == 12000) {
          m_vibrationControllerUnit.AdcUnit.stop();
          pidResponseNumber = 0;
        }
      }
    }
  }
}
int32_t ModuleController::setInitialStateADC() {
  uint16_t actualValue = 0;
  int32_t lastValue = 0;
  bool ready = false;

  int32_t sum = 0;
  int32_t measureCount = 0;

  auto abs = [](int32_t value) {
    if (value < 0) {
      return -value;
    }
    return value;
  };

  ADCCallbackConfig_t config{true, true}; // Actual value on, buffer false

  m_vibrationControllerUnit.AdcUnit.start(
      adcBuffer, COMMUNICATOR_BUFFER_MAX_SIZE, &actualValue, config,
      [&]([[maybe_unused]] ReceiveADCValueType_t type,
          [[maybe_unused]] size_t length) {
        if (measureCount < 1000) {
          auto value = static_cast<int32_t>(actualValue);
          int32_t difference = abs(value - lastValue);
          sum += difference;
          lastValue = value;
          measureCount++;
        } else {
          auto mean = static_cast<int32_t>(sum / 1000.0);
          if (mean < 20) {
            ready = true;
            m_vibrationControllerUnit.AdcUnit.stop();
            return;
          }
          sum = 0;
          measureCount = 0;
        }
      });

  while (!ready)
    ;

  measureCount = 0;
  sum = 0;
  while (true) {
    sum += actualValue;
    measureCount++;

    if (measureCount == 1000) {
      break;
    }

    osDelay(System::TicksFromMs(1));
  }
  return static_cast<int32_t>(sum / 1000.0);
}

Position ModuleController::convertPosition(uint8_t position) {
  switch (position) {
  case 0:
    return Position::First;
  case 1:
    return Position::Second;
  case 2:
    return Position::Third;
  case 3:
    return Position::Fourth;
  case 4:
    return Position::Fifth;
  case 5:
    return Position::Sixth;
  case 6:
    return Position::Seventh;
  case 7:
    return Position::Eighth;
  default:
    return Position::None;
  }
}

void ModuleController::goToPosition(Position position) {
  auto status = m_vibrationControllerUnit.goToPosition(position);
  while (status != VibrationController::OnPosition::True) {
    status = m_vibrationControllerUnit.goToPosition(position);
  }
}

void ModuleController::checkVibrationReduceProcedure(void *args) {
    etl::delegate<void(uint16_t *, size_t)> callback =
            *reinterpret_cast<etl::delegate<void(uint16_t *, size_t)> *>(
                    args);
    bool started = false;
    bool finished = false;
    uint16_t actualValue;
    size_t receivedValueCount = 0;

    ADCCallbackConfig_t config{true, true}; // Actual value on, buffer on

    decltype(m_pattern.begin()) patternIterator;

    auto executePatternElement = [=](const auto &patternElement) {
        auto command = static_cast<PatternCommandType>(patternElement.command);
        switch (command) {
            case PatternCommandType::Go:
                goToPosition(convertPosition(patternElement.content.go.position));
                break;
            case PatternCommandType::Wait:
                osDelay(System::TicksFromMs(patternElement.content.wait.time));
                break;
        }
    };

    while (true) {
        if (!started) {
            m_pattern.lock();
            started = true;
            patternIterator = m_pattern.begin();

            m_vibrationControllerUnit.goToInitialPosition();
            m_vibrationControllerUnit.AdcUnit.start(
                    adcBuffer, COMMUNICATOR_BUFFER_MAX_SIZE, &actualValue, config,
                    [&](ReceiveADCValueType_t type, size_t length) {
                        if (type == ReceiveADCValueType_t::Actual) {

                        } else {
                            receivedValueCount = length;
                            finished = true;
                        }
                    });
        }

        if (finished) {
            finished = false;
            started = false;
            m_vibrationControllerUnit.ElectricCoilsUnit.stop();
            m_vibrationControllerUnit.goToInitialPosition();
            m_pattern.unlock();
            callback(adcBuffer, receivedValueCount);
            checkPIDTask.suspendTask();
        }

        if (started) {
            if (patternIterator != m_pattern.end()) {
                const auto &element = *patternIterator;
                executePatternElement(element);
                patternIterator++;
            } else {
                m_vibrationControllerUnit.AdcUnit.stop();
            }
        }
    }
}