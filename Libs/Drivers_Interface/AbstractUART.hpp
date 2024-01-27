#ifndef __UART_HPP__
#define __UART_HPP__

#include <stdint.h>
#include <stddef.h>

#include <etl_profile.h>
#include <etl/delegate.h>

enum class CallbackType {
    RxInterrupt,
    TxInterrupt,
    RxDMA,
    TxDMA
};

//AbstractUART Interface
struct AbstractUART {
    //Send bytes via polling mode
    virtual void transmit(const uint8_t *data, uint32_t length) = 0;

    //Send bytes via Interrupt mode
    virtual void transmit_IT(const uint8_t *data, uint32_t length) = 0;

    //Send bytes via DMA mode
    virtual void transmit_DMA(const uint8_t *data, uint16_t length) = 0;

    //Receive bytes via Polling mode
    virtual void receive(uint8_t *data, uint32_t length) = 0;

    //Receive bytes via Interrupt mode
    virtual void receive_IT(uint8_t *data, uint32_t maxLength) = 0;

    //Receive bytes via DMA mode
    virtual void receive_DMA(uint8_t *data, uint16_t maxLength) = 0;

    virtual uint16_t DMABytesReceivedCount() const = 0;

    virtual inline void setCallback(etl::delegate<void(CallbackType)> &callback) = 0;

};


#endif /* __UART_HPP__ */