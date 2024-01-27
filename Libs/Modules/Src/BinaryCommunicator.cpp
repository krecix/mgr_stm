#include "BinaryCommunicator.hpp"

#include "System.hpp"
#include "etl_profile.h"
#include "etl/delegate.h"
#include "etl/crc32.h"
#include "Parser.hpp"


BinaryCommunicator BinaryCommunicator::instance;



static Buffer communicatorBuffer;

BinaryCommunicator::BinaryCommunicator()
        : uartPort{nullptr},
          receiveSemaphore{"BinaryReceiveSemaphore"},
          parseSemaphore{"BinaryParseSemaphore"} {

}

void BinaryCommunicator::Initialize(AbstractUART &uart) {
    uartPort = &uart;

    static auto uartCallback = [this](CallbackType callbackType) {
        this->UART_Callback(callbackType);
    };
    etl::delegate<void(CallbackType)> uartCallbackDelegate{uartCallback};
    uartPort->setCallback(uartCallbackDelegate);

    createReceiveTask();
    createParserTask();

}

void BinaryCommunicator::createReceiveTask() {
    static System::Task receiveBinaryBufferTask("ReceiveBinaryBufferTask", 1024, System::RTOSPriority::Normal1);
    static auto receiveCallbackTask = [](void *args) {
        auto object = reinterpret_cast<BinaryCommunicator *>(args);
        object->receiveTask();
    };
    receiveSemaphore.createSemaphore(1, 0);
    receiveBinaryBufferTask.createTask(static_cast<System::ThreadProcedure>(receiveCallbackTask), this);
}

void BinaryCommunicator::createParserTask() {
    static System::Task parserTask("ReceiveBinaryBufferTask", 1024, System::RTOSPriority::Normal1);
    static auto parserCallbackTask = [](void *args) {
        auto object = reinterpret_cast<BinaryCommunicator *>(args);
        object->bufferParserTask();
    };
    parseSemaphore.createSemaphore(1, 0);
    parserTask.createTask(static_cast<System::ThreadProcedure>(parserCallbackTask), this);
}

void BinaryCommunicator::receiveTask() {
    uartPort->receive_DMA(communicatorBuffer.get(), COMMUNICATOR_BUFFER_MAX_SIZE);

    while (true) {
        receiveSemaphore.get(System::WaitForever);

        Valid_t isCorrect;

        volatile auto bytesCount = uartPort->DMABytesReceivedCount();
        communicatorBuffer.setCount(bytesCount);

        if (bytesCount > 10) {
            isCorrect = checkMessage();
        } else {
            isCorrect = Valid_t::Incorrect;
        }

        if (isCorrect == Correct) {
            shiftBufferLeft(5);
            communicatorBuffer.setCount(communicatorBuffer.size() - 10);
            parseSemaphore.release();
        } else {
            prepareReceiveInvalidMessage();
            uartPort->transmit_DMA(communicatorBuffer.get(), communicatorBuffer.size());
        }
    }
}

void BinaryCommunicator::sendCurrentBuffer() {
    prepareBufferToSend();
    uartPort->transmit_DMA(communicatorBuffer.get(), communicatorBuffer.size());
}

constexpr uint8_t UnknownCommandMessage[]{0x01, 0x01, 0x00, 0x00, 0x00, 0xFE, 0x96, 0x30, 0x07, 0x88, 0x01};
constexpr uint32_t UnknownCommandMessageSize = sizeof(UnknownCommandMessage);
constexpr uint8_t InvalidCommandMessage[]{0x01, 0x01, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x01};
constexpr uint32_t InvalidCommandMessageSize = sizeof(InvalidCommandMessage);

void BinaryCommunicator::bufferParserTask() {

    while (true) {
        parseSemaphore.get(System::WaitForever);

        auto status = Parser::Parse(&communicatorBuffer);

        switch (status) {
            case ParseStatus_t::ValidCommandReadyToSend:
                sendCurrentBuffer();
                break;
            case ParseStatus_t::ValidCommandNoReadyToSend:
                break;
            case ParseStatus_t::UnknownCommand:
                communicatorBuffer.setCount(UnknownCommandMessageSize);
                for (uint32_t i = 0; i < UnknownCommandMessageSize; i++) {
                    communicatorBuffer.get()[i] = UnknownCommandMessage[i];
                }
                uartPort->transmit_DMA(communicatorBuffer.get(), communicatorBuffer.size());
                break;
            case ParseStatus_t::InvalidCommand:
                communicatorBuffer.setCount(InvalidCommandMessageSize);
                for (uint32_t i = 0; i < InvalidCommandMessageSize; i++) {
                    communicatorBuffer.get()[i] = InvalidCommandMessage[i];
                }
                uartPort->transmit_DMA(communicatorBuffer.get(), communicatorBuffer.size());
                break;
            default:
                break;
        }
    }
}

void BinaryCommunicator::UART_Callback(CallbackType callbackType) {
    switch (callbackType) {
        case CallbackType::RxDMA:
            receiveSemaphore.release();
            break;
        case CallbackType::TxDMA:
            uartPort->receive_DMA(communicatorBuffer.get(), COMMUNICATOR_BUFFER_MAX_SIZE);
            break;
        default:
            break;
    }
}

Valid_t BinaryCommunicator::checkMessage() {

    if (communicatorBuffer.get()[0] != 0x01) {
        return Incorrect;
    }

    actualDataInfo.start = 0x01;

    const auto length = *reinterpret_cast<uint32_t *>(&communicatorBuffer.get()[1]);

    actualDataInfo.length = length;

    actualDataInfo.dataPtr = &communicatorBuffer.get()[5];

    const auto crcIndex = 5 + length;

    actualDataInfo.crc32 = *reinterpret_cast<uint32_t *>(&communicatorBuffer.get()[crcIndex]);

    const auto stopByteIndex = crcIndex + 4;

    if (communicatorBuffer.get()[stopByteIndex] != 0x01) {
        return Incorrect;
    }

    const auto calculatedCRC32 = calculateCRC32(actualDataInfo.dataPtr, actualDataInfo.length);

    if (calculatedCRC32 != actualDataInfo.crc32) {
        return Incorrect;
    }

    return Correct;
}

uint32_t BinaryCommunicator::calculateCRC32(const uint8_t *buffer, size_t length) {

    etl::crc32 crc_calculator;

    std::copy(buffer, buffer + length, crc_calculator.input());

    return crc_calculator.value();
}


constexpr uint8_t InvalidMessage[]{0x01, 0x01, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x01};
constexpr uint32_t InvalidMessageSize = sizeof(InvalidMessage);

void BinaryCommunicator::prepareReceiveInvalidMessage() {

    communicatorBuffer.setCount(InvalidMessageSize);

    for (uint32_t i = 0; i < InvalidMessageSize; i++) {
        communicatorBuffer.get()[i] = InvalidMessage[i];
    }

}

void BinaryCommunicator::shiftBufferLeft(const size_t nIndexes) {
    for (size_t i = nIndexes; i < communicatorBuffer.size(); i++) {
        communicatorBuffer.get()[i - nIndexes] = communicatorBuffer.get()[i];
    }
}

void BinaryCommunicator::shiftBufferRight(const size_t nIndexes) {
    for (size_t i = communicatorBuffer.size() - 1; i > 0; i--) {
        communicatorBuffer.get()[i + nIndexes] = communicatorBuffer.get()[i];
    }
    communicatorBuffer.get()[nIndexes] = communicatorBuffer.get()[0];
}

void BinaryCommunicator::prepareBufferToSend() {

    auto calculatedCRC32 = calculateCRC32(communicatorBuffer.get(), communicatorBuffer.size());
    shiftBufferRight(5);

    communicatorBuffer.get()[0] = 0x01;

    *reinterpret_cast<uint32_t *>(&communicatorBuffer.get()[1]) = communicatorBuffer.size();

    *reinterpret_cast<uint32_t *>(&communicatorBuffer.get()[5 + communicatorBuffer.size()]) = calculatedCRC32;

    communicatorBuffer.get()[5 + communicatorBuffer.size() + 4] = 0x01;

    communicatorBuffer.setCount(communicatorBuffer.size() + 10);

}
