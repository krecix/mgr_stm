#include "STM32H7A3_UART.hpp"

#include <cmath>

namespace Drivers {

    enum class TransmissionMode {
        Idle,
        Interrupt,
        DMA
    };

    void STM32H7A3_UART::initialize() {
        uartPort->CR1 &= ~USART_CR1_UE;

        setBaudRate(baudRate);

        //DMA Rx
        if (rxStreamDMA != nullptr) {
            enableRxDMA();
        }

        //DMA Tx
        if (rxStreamDMA != nullptr) {
            enableTxDMA();
        }

        uartPort->CR3 |= USART_CR3_OVRDIS;

        uartPort->CR1 |= USART_CR1_IDLEIE;

        uartPort->CR1 |= USART_CR1_TE | USART_CR1_RE;

        uartPort->CR1 |= USART_CR1_UE;
    }

    void STM32H7A3_UART::enableRxDMA() {

        uartPort->CR3 |= USART_CR3_DMAR;

        // Priority Low, memory data size 8bit, peripherial data size 8bit, memory
        // increment mode transfer complete interrupt
        rxStreamDMA->CR |= DMA_SxCR_MINC | DMA_SxCR_TCIE;

        // Errata (DMA stream locked when transferring data to/from USART/AbstractUART)
        rxStreamDMA->CR |= (1UL << 20U);


    }

    void STM32H7A3_UART::enableTxDMA() {
        uartPort->CR3 |= USART_CR3_DMAT;

        // Priority Low, memory data size 8bit, peripherial data size 8bit, memory
        // increment mode transfer complete interrupt
        txStreamDMA->CR |= DMA_SxCR_MINC | DMA_SxCR_TCIE | DMA_SxCR_DIR_0;

        // Errata (DMA stream locked when transferring data to/from USART/AbstractUART)
        txStreamDMA->CR |= (1UL << 20U);
    }

    void STM32H7A3_UART::setBaudRate(uint32_t rate) {
        //Calculate BRR
        auto brrRaw = portFreq / (16.0 * rate);
        auto base = floor(brrRaw);
        auto fraction = floor((brrRaw - base) * 16.0);
        uartPort->BRR = (static_cast<uint32_t>(base) << 4UL) | static_cast<uint32_t>(fraction);

        this->baudRate = rate;
    }

//Send bytes via polling mode
    void STM32H7A3_UART::transmit(const uint8_t *data, uint32_t length) {
        for (size_t i = 0; i < length; i++) {
            //Transmit data register empty flag
            while (!(uartPort->ISR & (1UL << 7UL)));
            uartPort->TDR = data[i];
        }
        //Transmission complete flag
        while (!(uartPort->ISR & (1UL << 6UL)));
    }

    TransmissionMode transmitMode = TransmissionMode::Idle;


    static const uint8_t *sendDataPtr = nullptr;
    static size_t sendlengthData = 0;
    static size_t bytesSended = 0;

//Send bytes via Interrupt mode
    void STM32H7A3_UART::transmit_IT(const uint8_t *data, uint32_t length) {
        sendDataPtr = data;
        sendlengthData = length;
        bytesSended = 0;
        transmitMode = TransmissionMode::Interrupt;
        uartPort->TDR = data[0];
        uartPort->CR1 |= USART_CR1_TCIE;

    }

//Send bytes via DMA mode
    void STM32H7A3_UART::transmit_DMA(const uint8_t *data, uint16_t length) {
        if (txStreamDMA != nullptr) {
            transmitMode = TransmissionMode::DMA;
            txStreamDMA->M0AR = reinterpret_cast<uint32_t>(data);
            txStreamDMA->PAR = reinterpret_cast<uint32_t>(&uartPort->TDR);
            txStreamDMA->NDTR = length;
            txStreamDMA->CR |= DMA_SxCR_EN;
        } else {
            transmit_IT(data, length);
        }
    }

//Receive bytes via Polling mode
    void STM32H7A3_UART::receive(uint8_t *data, uint32_t length) {
        size_t bytesReceived = 0;
        while (bytesReceived < length) {
            //Read data register not empty flag
            if (uartPort->ISR & (1UL << 5UL)) {
                data[bytesReceived] = uartPort->RDR;
                bytesReceived++;
            }
        }
    }


    static TransmissionMode receiveMode = TransmissionMode::Idle;

    static uint8_t *receiveDataPtr = nullptr;
    static size_t receivelengthData = 0;
    static size_t bytesReceived = 0;

//Receive bytes via Interrupt mode
    void STM32H7A3_UART::receive_IT(uint8_t *data, uint32_t maxLength) {
        receiveDataPtr = data;
        receivelengthData = maxLength;
        bytesReceived = 0;
        receiveMode = TransmissionMode::Interrupt;
        uartPort->CR1 |= USART_CR1_RXNEIE;
    }

//Receive bytes via DMA mode
    void STM32H7A3_UART::receive_DMA(uint8_t *data, uint16_t maxLength) {
        if (rxStreamDMA != nullptr) {
            receiveMode = TransmissionMode::DMA;

            actualMaxDMAReceivedBytes = maxLength;

            rxStreamDMA->PAR = reinterpret_cast<uint32_t>(&uartPort->RDR);
            rxStreamDMA->M0AR = reinterpret_cast<uint32_t>(data);
            rxStreamDMA->NDTR = maxLength;
            rxStreamDMA->CR |= DMA_SxCR_EN;
        } else {
            receive_IT(data, maxLength);
        }

    }

    uint16_t STM32H7A3_UART::DMABytesReceivedCount() const {
        return actualMaxDMAReceivedBytes - rxStreamDMA->NDTR;
    }

    void STM32H7A3_UART::Interrupt() {

        //Transmit data register empty flag
        if ((uartPort->ISR & (1UL << 7UL))) {
            transmitDataEmptyInt();
        }
        //Transmission complete flag
        if ((uartPort->ISR & (1UL << 6UL))) {
            transmitCompleteInt();
        }

        //Receive flag
        if ((uartPort->ISR & (1UL << 5UL))) {
            receiveInt();
        }

        //Idle flag
        if ((uartPort->ISR & (1UL << 4UL))) {
            idleInt();
        }
    }

    void STM32H7A3_UART::transmitDataEmptyInt() {
        if (transmitMode == TransmissionMode::Interrupt) {
            bytesSended++;
            if (bytesSended < sendlengthData) {
                uartPort->TDR = sendDataPtr[bytesSended];
            }
        }

    }

    void STM32H7A3_UART::transmitCompleteInt() {
        uartPort->ICR |= USART_ICR_TCCF;
        uartPort->CR1 &= ~USART_CR1_TCIE;
        if (transmitMode == TransmissionMode::Interrupt) {
            transmitMode = TransmissionMode::Idle;
            callback(CallbackType::TxInterrupt);
        } else if (transmitMode == TransmissionMode::DMA) {
            transmitMode = TransmissionMode::Idle;
            callback(CallbackType::TxDMA);
        }
    }

    void STM32H7A3_UART::receiveInt() {
        uint8_t byte = uartPort->RDR;

        if (receiveMode == TransmissionMode::Interrupt) {
            receiveDataPtr[bytesReceived] = byte;
            bytesReceived++;
            if (bytesReceived >= receivelengthData) {
                receiveDataPtr[bytesReceived] = byte;
                bytesReceived++;
                uartPort->CR1 &= ~USART_CR1_RXNEIE;
                callback(CallbackType::RxInterrupt);
            }
        }

    }

    void STM32H7A3_UART::idleInt() {
        uartPort->ICR |= USART_ICR_IDLECF;
        if (receiveMode == TransmissionMode::Interrupt) {
            uartPort->CR1 &= ~USART_CR1_RXNEIE;
            callback(CallbackType::RxInterrupt);
        } else if (receiveMode == TransmissionMode::DMA) {
            //Disable DMA and generate interrupt
            rxStreamDMA->CR &= ~DMA_SxCR_EN;
        }
    }


}


