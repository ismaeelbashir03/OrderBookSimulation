#pragma once

#include "Types.h"
#include <vector>

struct TradeInfo {
    OrderId orderId;
    Price price;
    Quantity quantity;
};

class Trade {
public:
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade) {
        this->bidTrade = bidTrade;
        this->askTrade = askTrade;
    }

    TradeInfo getBidTrade() {
        return bidTrade;
    }
    TradeInfo getAskTrade() {
        return askTrade;
    }

private:
    TradeInfo bidTrade;
    TradeInfo askTrade;
};

using Trades = std::vector<Trade>;
using OrderConfirmation = std::pair<OrderId, Trades>;