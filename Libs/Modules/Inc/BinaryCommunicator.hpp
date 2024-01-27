#ifndef __BinaryCommunicator_HPP__
#define __BinaryCommunicator_HPP__

#include <etl_profile.h>
#include <etl/circular_buffer.h>

#include "AbstractUART.hpp"
#include "Config.hpp"
#include "System.hpp"
#include "Buffer.hpp"


enum Valid_t {
    Incorrect,
    Correct
};

struct BinaryData_t {
    uint8_t start, stop;
    uint32_t length;
    uint8_t *dataPtr;
    uint32_t crc32;
};

class BinaryCommunicator final {
public:

    BinaryCommunicator(const BinaryCommunicator &other) = delete;

    BinaryCommunicator(BinaryCommunicator &&other) = delete;

    BinaryCommunicator &operator=(const BinaryCommunicator &other) = delete;

    BinaryCommunicator &operator=(BinaryCommunicator &&other) = delete;

    virtual ~BinaryCommunicator() = default;

    static inline BinaryCommunicator &Instance() {
        return instance;
    }

    void Initialize(AbstractUART &uart);

    void sendCurrentBuffer();

    static uint32_t calculateCRC32(const uint8_t *buffer, size_t length);

private:

    BinaryCommunicator();

    [[noreturn]] void receiveTask();

    void createReceiveTask();

    [[noreturn]] void bufferParserTask();

    void createParserTask();

    void UART_Callback(CallbackType callbackType);

    Valid_t checkMessage();

    void prepareReceiveInvalidMessage();

    void shiftBufferLeft(const size_t nIndexes);

    void shiftBufferRight(const size_t nIndexes);

    void prepareBufferToSend();

    static BinaryCommunicator instance;

    AbstractUART *uartPort;

    System::Semaphore receiveSemaphore;
    System::Semaphore parseSemaphore;

    BinaryData_t actualDataInfo;

};


#endif /* __BinaryCommunicator_HPP__ */
