#pragma once

#include "Types.h"

class OrderBookLevelInfos {
public:
    OrderBookLevelInfos(const LevelInfos& bids, const LevelInfos& asks);

    LevelInfos getBids() const;
    LevelInfos getAsks() const;

private:
    LevelInfos bids;
    LevelInfos asks;
};