#pragma once

#include "Types.h"

class OrderBookLevelInfos {
public:
    OrderBookLevelInfos(const LevelInfos& bids, const LevelInfos& asks) {
        this->bids = bids;
        this->asks = asks;
    }

    LevelInfos getBids() {
        return bids;
    };
    LevelInfos getAsks() {
        return asks;
    };

private:
    LevelInfos bids;
    LevelInfos asks;
};