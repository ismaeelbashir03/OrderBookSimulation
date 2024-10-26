#include "Orderbook.h"
#include <iostream>

Orderbook::Orderbook() : orderPool(1000) {}  // Preallocate memory for 1000 Order objects

void Orderbook::printOrderbook() {
    OrderBookLevelInfos orderInfos = getOrderInfos();

    std::cout << "Orderbook State: " << std::endl;
    std::cout << std::endl;

    std::cout << "Bid Levels:" << std::endl;
    for (const auto& level : orderInfos.getBids()) {
        std::cout << "Price: " << level.price << ", Quantity: " << level.quantity << std::endl;
    }

    std::cout << "Ask Levels:" << std::endl;
    for (const auto& level : orderInfos.getAsks()) {
        std::cout << "Price: " << level.price << ", Quantity: " << level.quantity << std::endl;
    }

    std::cout << std::endl;
}

OrderConfirmation Orderbook::addOrder(const Price price, const Quantity quantity, const Side side, const OrderType orderType) {
    OrderPtr order = orderPool.allocate();
    *order = Order(getOrderId(), price, quantity, side, orderType);

    // if we can't match the FoK order, return
    if (order->getOrderType() == OrderType::FillOrKill) {
        if (!canMatch(order->getSide(), order->getPrice())) {
            orderPool.deallocate(order); // cleanup
            return {};
        }
    }

    // if the price level doesn't exist, create it
    if (order->getSide() == Side::Buy && bidLevels.find(order->getPrice()) == bidLevels.end()) {
        auto q = std::make_shared<Queue>();
        bidLevels[order->getPrice()] = std::move(q);
        bids.push(std::make_pair(order->getPrice(), bidLevels[order->getPrice()]));
    } else if (order->getSide() == Side::Sell && askLevels.find(order->getPrice()) == askLevels.end()) {
        auto q = std::make_shared<Queue>();
        askLevels[order->getPrice()] = std::move(q);
        asks.push(std::make_pair(order->getPrice(), askLevels[order->getPrice()]));
    }

    // add the order to the price level
    QueuePtr queue = (order->getSide() == Side::Buy) ? bidLevels[order->getPrice()] : askLevels[order->getPrice()];
    queue->push_back(order);

    // store order and its location
    orders[order->getOrderId()] = order;

    return OrderConfirmation{order->getOrderId(), matchOrders()};
}

void Orderbook::cancelOrder(OrderId orderId) {
    // if the order doesn't exist, return
    if (orders.find(orderId) == orders.end()) {
        return;
    }

    // remove the order from the map
    auto order = orders[orderId];
    orders.erase(orderId);

    // remove the order from the price level
    auto& level = (order->getSide() == Side::Buy) ? bidLevels[order->getPrice()] : askLevels[order->getPrice()];
    auto it = std::find(level->begin(), level->end(), order);
    if (it != level->end()) {
        level->erase(it);
    }

    // not removing the level from the heap if empty,
    // as it will be removed when it is at top (lazy deletion)

    // deallocate the order
    orderPool.deallocate(order);
}

OrderConfirmation Orderbook::modifyOrder(OrderId orderId, Price price, Quantity quantity, Side side) {
    // if the order doesn't exist, return
    if (orders.find(orderId) == orders.end()) {
        return {};
    }

    // cant change the order type, so this should be stored
    OrderType orderType = orders[orderId]->getOrderType();

    // cancel the order
    cancelOrder(orderId);

    // add the modified order
    return addOrder(price, quantity, side, orderType);
}

std::size_t Orderbook::getNumBids() const {
    return bids.size();
}

std::size_t Orderbook::getNumAsks() const {
    return asks.size();
}

std::size_t Orderbook::getNumOrders() const {
    return orders.size();
}

OrderBookLevelInfos Orderbook::getOrderInfos() const {
    LevelInfos bidInfos, askInfos;

    bidInfos.reserve(orders.size());
    askInfos.reserve(orders.size());

    auto CreateLevelInfos = [](Price price, const QueuePtr& q) {
        Quantity totalQuantity = 0;
        for (const auto& order : *q) {
            totalQuantity += order->getRemainingQuantity();
        }
        return LevelInfo{price, totalQuantity};
    };

    auto HeapToVector = [](const auto& heap) {
        std::vector<std::pair<Price, QueuePtr>> vec;
        auto copy = heap;
        while (!copy.empty()) {
            vec.push_back(std::make_pair(copy.top().first, copy.top().second));
            copy.pop();
        }
        return vec;
    };

    auto bidVec = HeapToVector(bids);
    auto askVec = HeapToVector(asks);

    for (const auto& [price, q] : bidVec) {
        bidInfos.push_back(CreateLevelInfos(price, q));
    }

    for (const auto& [price, q] : askVec) {
        askInfos.push_back(CreateLevelInfos(price, q));
    }

    return OrderBookLevelInfos{bidInfos, askInfos};
}

int Orderbook::getOrderId() {
    return orderId++;
}

bool Orderbook::canMatch(Side side, Price price) const {
    if (side == Side::Buy) {
        // if there are no asks, we can't match
        // if the price is less than the best ask, we can't match
        return !asks.empty() && price >= asks.top().first;
    } else {
        // if there are no bids, we can't match
        // if the price is greater than the best bid, we can't match
        return !bids.empty() && price <= bids.top().first;
    }
}

Trades Orderbook::matchOrders() {
    Trades trades;
    trades.reserve(asks.size() + bids.size()); // At most, we can have one trade per order

    // handle lazy deletion of levels (if they are empty)
    while (!bids.empty() && bidLevels[bids.top().first]->empty()) {
        bidLevels.erase(bids.top().first);
        bids.pop();
    }
    while (!asks.empty() && askLevels[asks.top().first]->empty()) {
        askLevels.erase(asks.top().first);
        asks.pop();
    }

    while (!bids.empty() && !asks.empty()) {
        Price bidPrice = bids.top().first;
        auto& bidLevel = bids.top().second;
        if (bidLevel->empty()) {
            bids.pop();
            bidLevels.erase(bidPrice);
            continue;
        }
        OrderPtr topBid = bidLevel->front();

        Price askPrice = asks.top().first;
        auto& askLevel = asks.top().second;
        if (askLevel->empty()) {
            asks.pop();
            askLevels.erase(askPrice);
            continue;
        }
        OrderPtr topAsk = askLevel->front();

        // if the bid price is less than the ask price, we can't match
        if (bidPrice < askPrice) {
            break;
        }

        Quantity tradeQuantity = std::min(topBid->getRemainingQuantity(), topAsk->getRemainingQuantity());
        topBid->fill(tradeQuantity);
        topAsk->fill(tradeQuantity);

        trades.push_back(
            Trade{
                TradeInfo{topBid->getOrderId(), topBid->getPrice(), tradeQuantity},
                TradeInfo{topAsk->getOrderId(), topAsk->getPrice(), tradeQuantity}
            }
        );

        // if the order is fully filled, remove it from the level
        if (topAsk->getRemainingQuantity() == 0) {
            askLevel->pop_front();
            if (askLevel->empty()) {
                asks.pop();
                askLevels.erase(topAsk->getPrice());
            }
            orders.erase(topAsk->getOrderId());
            orderPool.deallocate(topAsk);
        }

        if (topBid->getRemainingQuantity() == 0) {
            bidLevel->pop_front();
            if (bidLevel->empty()) {
                bids.pop();
                bidLevels.erase(topBid->getPrice());
            }
            orders.erase(topBid->getOrderId());
            orderPool.deallocate(topBid);
        }
    }
    return trades;
}