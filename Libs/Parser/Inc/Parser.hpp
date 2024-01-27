//
// Created by krzysztof on 27.03.2021.
//

#ifndef VIBRATIONCONTROLSTM32_PARSER_HPP
#define VIBRATIONCONTROLSTM32_PARSER_HPP

#include "etl_profile.h"
#include "etl/delegate.h"

#include "Config.hpp"
#include "Buffer.hpp"
#include "ModuleController.hpp"

enum class Commands_t : uint8_t {
    WhoIAm = 0x0A,
    DeviceParameters = 0x0B,
    Pattern = 0x0C,
    CheckPattern = 0x0D,
    CheckPID = 0x0E
};

enum class ParseStatus_t {
    UnknownCommand,
    InvalidCommand,
    ValidCommandReadyToSend,
    ValidCommandNoReadyToSend
};

enum class SetPatternStatus_t {
    Ok = 0x01,
    TooLong = 0x02,
    TooLongMs = 0x03,
    BadStructure = 0x04,
    InUse = 0x05
};

enum class CheckPatternStatus_t {
    Ok = 0x01,
    InvalidPattern = 0x02,
    UndefinedError = 0x03,
    InUse = 0x04
};

enum class CheckPIDStatus_t {
    Ok = 0x01,
    InvalidPattern = 0x02,
    UndefinedError = 0x03,
    InUse = 0x04
};

constexpr auto BUFFER_SPACE_RESERVE = 20;

class Parser final {
public:
    Parser() = delete;

    Parser(const Parser &other) = delete;

    Parser(Parser &&other) = delete;

    Parser &operator=(const Parser &other) = delete;

    Parser &operator=(Parser &&other) = delete;

    static ParseStatus_t Parse(Buffer *buffer);

    static void setModuleController(ModuleController *moduleController) {
        m_moduleController = moduleController;
    }

private:

    static ParseStatus_t WhoIAmCMD();

    static ParseStatus_t DeviceParametersCMD();

    static ParseStatus_t SetPatternCMD();

    static ParseStatus_t CheckPatternCMD();

    static SetPatternStatus_t setPattern(size_t patternLength);

    static ParseStatus_t CheckPID_CMD();

    static inline ModuleController *m_moduleController;

    static inline Buffer *m_buffer;

};


#endif //VIBRATIONCONTROLSTM32_PARSER_HPP
