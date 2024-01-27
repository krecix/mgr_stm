#ifndef __STM32H7A3_UART_HPP__
#define __STM32H7A3_UART_HPP__

#include "AbstractUART.hpp"
#include "stm32h7a3xxq.h"

namespace Drivers {

    using USART_t = USART_TypeDef *;

    using DMA_Stream_t = DMA_Stream_TypeDef *;

    class STM32H7A3_UART final : public AbstractUART {
    public:
        explicit STM32H7A3_UART(USART_t port, uint32_t portFreq, uint32_t baudRate,
                                DMA_Stream_t rxDMAStream, DMA_Stream_t txDMAStream)
                : uartPort{port},
                  portFreq{portFreq},
                  baudRate{baudRate},
                  rxStreamDMA{rxDMAStream},
                  actualMaxDMAReceivedBytes{0},
                  txStreamDMA{txDMAStream} {

        }

        STM32H7A3_UART(const STM32H7A3_UART &other) = delete;

        STM32H7A3_UART(STM32H7A3_UART &&other) = delete;

        STM32H7A3_UART &operator=(const STM32H7A3_UART &other) = delete;

        STM32H7A3_UART &operator=(STM32H7A3_UART &&other) = delete;

        void initialize();

        virtual ~STM32H7A3_UART() = default;

        //Send bytes via polling mode
        void transmit(const uint8_t *data, uint32_t length) override;

        //Send bytes via Interrupt mode
        void transmit_IT(const uint8_t *data, uint32_t length) override;

        //Send bytes via DMA mode
        void transmit_DMA(const uint8_t *data, uint16_t length) override;

        //Receive bytes via Polling mode
        void receive(uint8_t *data, uint32_t length) override;

        //Receive bytes via Interrupt mode
        void receive_IT(uint8_t *data, uint32_t maxLength) override;


        //Receive bytes via DMA mode
        void receive_DMA(uint8_t *data, uint16_t maxLength) override;

        uint16_t DMABytesReceivedCount() const override;

        void setBaudRate(uint32_t rate);

        void Interrupt();

        inline void txDMAInterrupt_TransferComplete() {
            uartPort->CR1 |= USART_CR1_TCIE;
        }

        inline void rxDMAInterrupt_TransferComplete() {
            callback(CallbackType::RxDMA);
        }

        inline void setCallback(etl::delegate<void(CallbackType)> &callback) override {
            callbackFunc = callback;
        }

        inline void callback(CallbackType callbackType) {
            if (callbackFunc.is_valid()) {
                callbackFunc(callbackType);
            }
        }

    private:

        void enableRxDMA();

        void enableTxDMA();

        void transmitDataEmptyInt();

        void transmitCompleteInt();

        void receiveInt();

        void idleInt();


        USART_t uartPort;
        const uint32_t portFreq;
        uint32_t baudRate;

        DMA_Stream_t rxStreamDMA;
        uint16_t actualMaxDMAReceivedBytes;
        DMA_Stream_t txStreamDMA;

        etl::delegate<void(CallbackType)> callbackFunc;


    };


}

#endif /* __STM32H7A3_UART_HPP__ */