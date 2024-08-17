#include "Trade.h"

Trade::Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade)
    : bidTrade(bidTrade), askTrade(askTrade) {}

TradeInfo Trade::getBidTrade() const {
    return bidTrade;
}

TradeInfo Trade::getAskTrade() const {
    return askTrade;
}