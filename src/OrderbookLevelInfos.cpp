#include "OrderbookLevelInfos.h"

OrderBookLevelInfos::OrderBookLevelInfos(const LevelInfos& bids, const LevelInfos& asks) : bids(bids), asks(asks) {}

LevelInfos OrderBookLevelInfos::getBids() const {
    return bids;
}

LevelInfos OrderBookLevelInfos::getAsks() const {
    return asks;
}