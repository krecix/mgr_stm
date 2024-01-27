//
// Created by krzysztof on 28.03.2021.
//

#ifndef VIBRATIONCONTROLSTM32_PATTERN_HPP
#define VIBRATIONCONTROLSTM32_PATTERN_HPP

#include "Config.hpp"

#include "etl/vector.h"

enum class PatternCommandType : uint8_t {
    Go = 0x01,
    Wait = 0x02
};

struct PatternElement {
    uint8_t command;
    union {
        struct {
            uint8_t position;
            uint8_t startSpeedPercentage;
            uint8_t accelerationPercentagePer10Ms;
        } go;
        struct {
            uint16_t time;
        } wait;
    } content;
};

class Pattern {
public:
    explicit Pattern() = default;

    Pattern(const Pattern &other) = delete;

    Pattern(Pattern &&other) = delete;

    Pattern &operator=(const Pattern &other) = delete;

    Pattern &operator=(Pattern &&other) = delete;

    auto lock() {
        isLocked = true;
    }

    auto unlock() {
        isLocked = false;
    }

    auto locked() {
        return isLocked;
    }

    auto clear() {
        patternVector.clear();
    }

    auto appendElement(const PatternElement &element) {
        patternVector.push_back(element);
    }

    auto at(size_t index) {
        return patternVector.at(index);
    }

    auto operator[](size_t index) {
        return patternVector[index];
    }

    auto begin() {
        return patternVector.begin();
    }

    auto end() {
        return patternVector.end();
    }

    auto empty() {
        return patternVector.empty();
    }

    auto size() {
        return patternVector.size();
    }

    auto capacity() {
        return patternVector.capacity();
    }

private:

    etl::vector<PatternElement, MAX_PATTERN_LENGTH> patternVector;

    bool isLocked = false;
};


#endif //VIBRATIONCONTROLSTM32_PATTERN_HPP
