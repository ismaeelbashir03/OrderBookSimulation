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
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade);

    TradeInfo getBidTrade() const;
    TradeInfo getAskTrade() const;

private:
    TradeInfo bidTrade;
    TradeInfo askTrade;
};

using Trades = std::vector<Trade>;