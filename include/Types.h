#pragma once

#include <cstdint>
#include <vector>
#include <chrono>

// Order types
using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;
using Time = std::chrono::time_point<std::chrono::system_clock>;

// Level types
struct LevelInfo {
    Price price;
    Quantity quantity;
};

using LevelInfos = std::vector<LevelInfo>;