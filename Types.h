#pragma once

#include <cstdint>
#include <vector>

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

struct LevelInfo {
    Price price;
    Quantity quantity;
};

using LevelInfos = std::vector<LevelInfo>;