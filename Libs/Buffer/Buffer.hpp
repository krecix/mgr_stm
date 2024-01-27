//
// Created by krzysztof on 27.03.2021.
//

#ifndef VIBRATIONCONTROLSTM32_BUFFER_HPP
#define VIBRATIONCONTROLSTM32_BUFFER_HPP

#include "Config.hpp"

class Buffer {
public:
    Buffer()
            : buffer{0},
              bufferCount{0} {

    }

    uint8_t *get() {
        return buffer;
    }

    auto setCount(const size_t count) {
        bufferCount = count;
    }

    [[nodiscard]] auto size() const {
        return bufferCount;
    }

    [[nodiscard]] auto capacity() const {
        return COMMUNICATOR_BUFFER_MAX_SIZE;
    }

private:

    uint8_t buffer[COMMUNICATOR_BUFFER_MAX_SIZE];
    size_t bufferCount;
};

#endif //VIBRATIONCONTROLSTM32_BUFFER_HPP
