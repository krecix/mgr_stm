#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <cstdint>
#include <cstddef>

#include <cassert>

#include "etl_profile.h"
#include <etl/limits.h>

constexpr size_t SAMPLE_FREQUENCY = 1000UL;
constexpr size_t BINARY_PACKET_LENGTH = 10UL;
constexpr size_t MAX_PATTERN_LENGTH = 200UL;
constexpr size_t MAX_PATTERN_TIME_MS = 10'000UL;
constexpr size_t MAX_GENE_COUNT = 900UL;

constexpr size_t COMMUNICATOR_BUFFER_MAX_SIZE = 60'000UL + BINARY_PACKET_LENGTH;

static_assert(COMMUNICATOR_BUFFER_MAX_SIZE <= etl::integral_limits<uint16_t>::max, "Communicator buffer is too long!");
static_assert(((MAX_PATTERN_TIME_MS * (SAMPLE_FREQUENCY / 1000.0)) * 2.0) <= COMMUNICATOR_BUFFER_MAX_SIZE,
              "Pattern time is too long!");

#endif /* __CONFIG_HPP__ */
