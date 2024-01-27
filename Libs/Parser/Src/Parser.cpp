//
// Created by krzysztof on 27.03.2021.
//

#include "../Inc/Parser.hpp"

#include <cstdint>
#include <cstring>

#include "BinaryCommunicator.hpp"

ParseStatus_t Parser::Parse(Buffer *buffer) {

    m_buffer = buffer;

    auto command = static_cast<Commands_t>(buffer->get()[0]);

    switch (command) {
        case Commands_t::WhoIAm:
            return WhoIAmCMD();
        case Commands_t::DeviceParameters:
            return DeviceParametersCMD();
        case Commands_t::Pattern:
            return SetPatternCMD();
        case Commands_t::CheckPattern:
            return CheckPatternCMD();
        case Commands_t::CheckPID:
            return CheckPID_CMD();
        default:
            return ParseStatus_t::UnknownCommand;
    }
}

constexpr uint8_t WhoIAmMessage[]{static_cast<uint8_t>(Commands_t::WhoIAm), 'N', 'u', 'c', 'l', 'e', 'o', '-', 'H', '7',
                                  'A', '3'};
constexpr uint32_t WhoIAmMessageSize = sizeof(WhoIAmMessage);

ParseStatus_t Parser::WhoIAmCMD() {
    m_buffer->setCount(WhoIAmMessageSize);
    for (uint32_t i = 0; i < WhoIAmMessageSize; i++) {
        m_buffer->get()[i] = WhoIAmMessage[i];
    }
    return ParseStatus_t::ValidCommandReadyToSend;
}

struct Parameters {
    uint8_t address;
    uint32_t sampleFrequency;
    uint32_t communicatorMaxBufferSize;
    uint32_t maxPatternLength;
    uint32_t maxPatternTimeMs;
    uint32_t maxGeneCount;
} constexpr DeviceParametersMessage{
        static_cast<uint8_t>(Commands_t::DeviceParameters),
        SAMPLE_FREQUENCY,
        COMMUNICATOR_BUFFER_MAX_SIZE,
        MAX_PATTERN_LENGTH,
        MAX_PATTERN_TIME_MS,
        MAX_GENE_COUNT
};
constexpr size_t DeviceParametersMessageSize = sizeof(DeviceParametersMessage);

ParseStatus_t Parser::DeviceParametersCMD() {
    m_buffer->setCount(21);
    auto deviceParametersPointer = reinterpret_cast<const uint8_t *>(&DeviceParametersMessage);
    for (size_t i = 0; i < DeviceParametersMessageSize; i++) {
        m_buffer->get()[i] = deviceParametersPointer[i];
    }
}

ParseStatus_t Parser::SetPatternCMD() {
    if (m_buffer->size() <= 5) {
        return ParseStatus_t::InvalidCommand;
    }

    if (m_moduleController->getPattern().locked()) {
        m_buffer->setCount(2);
        m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::Pattern);
        m_buffer->get()[1] = static_cast<uint8_t>(SetPatternStatus_t::InUse);
        return ParseStatus_t::ValidCommandReadyToSend;
    }

    auto patternSize = *reinterpret_cast<uint32_t *>(&m_buffer->get()[1]); // 1 -> Pattern length index
    if (patternSize > m_moduleController->getPattern().capacity()) {
        m_buffer->setCount(2);
        m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::Pattern);
        m_buffer->get()[1] = static_cast<uint8_t>(SetPatternStatus_t::TooLong);
        return ParseStatus_t::ValidCommandReadyToSend;
    }

    auto status = setPattern(patternSize);

    m_buffer->setCount(2);
    m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::Pattern);
    m_buffer->get()[1] = static_cast<uint8_t>(status);
    return ParseStatus_t::ValidCommandReadyToSend;
}

SetPatternStatus_t Parser::setPattern(size_t patternLength) {
    m_moduleController->getPattern().clear();

    uint32_t patternTime = 0;
    size_t actualIndex = 5; // 10 -> Start pattern index
    PatternElement patternElement{};
    while (actualIndex + 3 <= m_buffer->size()) {
        auto commandType = m_buffer->get()[actualIndex];
        patternElement.command = commandType;
        if (static_cast<PatternCommandType>(commandType) == PatternCommandType::Go) {
            patternElement.content.go.position = m_buffer->get()[actualIndex + 1];
            patternElement.content.go.startSpeedPercentage = m_buffer->get()[actualIndex + 2];
            patternElement.content.go.accelerationPercentagePer10Ms = m_buffer->get()[actualIndex + 3];
            if (patternElement.content.go.position > 7) {
                m_moduleController->getPattern().clear();
                return SetPatternStatus_t::BadStructure;
            }
            if (patternElement.content.go.startSpeedPercentage > 100) {
                m_moduleController->getPattern().clear();
                return SetPatternStatus_t::BadStructure;
            }
            if (patternElement.content.go.accelerationPercentagePer10Ms > 100) {
                m_moduleController->getPattern().clear();
                return SetPatternStatus_t::BadStructure;
            }
            actualIndex += 4; //Increment actual index to next 4 bytes
        } else // PatternCommandType::Wait
        {
            patternElement.content.wait.time = *reinterpret_cast<uint16_t *>(&m_buffer->get()[actualIndex + 1]);
            patternTime += patternElement.content.wait.time;
            if (patternTime > MAX_PATTERN_TIME_MS) {
                m_moduleController->getPattern().clear();
                return SetPatternStatus_t::TooLongMs;
            }
            actualIndex += 3; //Increment actual index to next 3 bytes
        }
        m_moduleController->getPattern().appendElement(patternElement);
    }
    if (m_moduleController->getPattern().size() != patternLength) {
        return SetPatternStatus_t::BadStructure;
    }

    return SetPatternStatus_t::Ok;
}


ParseStatus_t Parser::CheckPatternCMD() {
    if (m_buffer->size() != 1) {
        return ParseStatus_t::InvalidCommand;
    }

    auto status = m_moduleController->checkPattern([&](uint16_t setpoint, uint16_t *measurement, size_t length) {

        if (length > (m_buffer->capacity() - BUFFER_SPACE_RESERVE)) {
            m_buffer->setCount(2);
            m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::CheckPattern);
            m_buffer->get()[1] = static_cast<uint8_t>(CheckPatternStatus_t::UndefinedError);
            BinaryCommunicator::Instance().sendCurrentBuffer();
            return;
        }

        m_buffer->setCount(4 + (length * 2));
        m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::CheckPattern);
        m_buffer->get()[1] = static_cast<uint8_t>(CheckPatternStatus_t::Ok);
        auto setPointBytesPtr = reinterpret_cast<const uint8_t*>(&setpoint);
        m_buffer->get()[2] = setPointBytesPtr[0];
        m_buffer->get()[3] = setPointBytesPtr[1];
        for (size_t i = 0; i < length; i++) {
            m_buffer->get()[4 + (i * 2)] = measurement[i] & 0xFF;
            m_buffer->get()[4 + ((i * 2) + 1)] = (measurement[i] >> 8UL) & 0xFF;
        }
        BinaryCommunicator::Instance().sendCurrentBuffer();

    });

    if (status == ModuleControllerStatus_t::InvalidPattern) {
        m_buffer->setCount(2);
        m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::CheckPattern);
        m_buffer->get()[1] = static_cast<uint8_t>(CheckPatternStatus_t::InvalidPattern);
        return ParseStatus_t::ValidCommandReadyToSend;
    } else if (status == ModuleControllerStatus_t::InUse) {
        m_buffer->setCount(2);
        m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::CheckPattern);
        m_buffer->get()[1] = static_cast<uint8_t>(CheckPatternStatus_t::InUse);
        return ParseStatus_t::ValidCommandReadyToSend;
    }

    return ParseStatus_t::ValidCommandNoReadyToSend;
}


ParseStatus_t Parser::CheckPID_CMD() {
    if (m_buffer->size() != 13) {
        return ParseStatus_t::InvalidCommand;
    }

    auto bytesToFloat = [](uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
        uint8_t byte_array[] = { b3, b2, b1, b0 };
        float result;
        std::copy(reinterpret_cast<const int8_t*>(&byte_array[0]),
                  reinterpret_cast<const int8_t*>(&byte_array[4]),
                  reinterpret_cast<int8_t*>(&result));
        return result;
    };

    float P = 0.0;
    float I = 0.0;
    float D = 0.0;

    auto bufferContentPtr = m_buffer->get();
    P = bytesToFloat(bufferContentPtr[1], bufferContentPtr[2], bufferContentPtr[3], bufferContentPtr[4]);
    I = bytesToFloat(bufferContentPtr[5], bufferContentPtr[6], bufferContentPtr[7], bufferContentPtr[8]);
    D = bytesToFloat(bufferContentPtr[9], bufferContentPtr[10], bufferContentPtr[11], bufferContentPtr[12]);

    auto status = m_moduleController->checkPID(P, I, D, [&](uint16_t setpoint, uint16_t *measurement, size_t length) {
        if (length > (m_buffer->capacity() - BUFFER_SPACE_RESERVE)) {
            m_buffer->setCount(2);
            m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::CheckPID);
            m_buffer->get()[1] = static_cast<uint8_t>(CheckPIDStatus_t::UndefinedError);
            BinaryCommunicator::Instance().sendCurrentBuffer();
            return;
        }

        m_buffer->setCount(4 + (length * 2));
        m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::CheckPID);
        m_buffer->get()[1] = static_cast<uint8_t>(CheckPIDStatus_t::Ok);
        auto setPointBytesPtr = reinterpret_cast<const uint8_t*>(&setpoint);
        m_buffer->get()[2] = setPointBytesPtr[0];
        m_buffer->get()[3] = setPointBytesPtr[1];
        for (size_t i = 0; i < length; i++) {
            m_buffer->get()[4 + (i * 2)] = measurement[i] & 0xFF;
            m_buffer->get()[4 + ((i * 2) + 1)] = (measurement[i] >> 8UL) & 0xFF;
        }
        BinaryCommunicator::Instance().sendCurrentBuffer();

    });

    if (status == ModuleControllerStatus_t::InvalidPattern) {
        m_buffer->setCount(2);
        m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::CheckPID);
        m_buffer->get()[1] = static_cast<uint8_t>(CheckPIDStatus_t::InvalidPattern);
        return ParseStatus_t::ValidCommandReadyToSend;
    } else if (status == ModuleControllerStatus_t::InUse) {
        m_buffer->setCount(2);
        m_buffer->get()[0] = static_cast<uint8_t>(Commands_t::CheckPID);
        m_buffer->get()[1] = static_cast<uint8_t>(CheckPIDStatus_t::InUse);
        return ParseStatus_t::ValidCommandReadyToSend;
    }

    return ParseStatus_t::ValidCommandNoReadyToSend;
}
