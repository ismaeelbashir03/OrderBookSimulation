#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <numeric>
#include "Order.h"
#include "OrderLevel.h"
#include "Trade.h"
#include "OrderModify.h"
#include "OrderbookLevelInfos.h"

/*
* Orderbook class

* Architecture:
* - Orders are stored in a map from order id to order ptr
* - Bids and Asks are stored in a max heap and min heap respectively
* - Price levels are a queue of orders in each entry of the heaps
*
* The Orderbook class is responsible for:
* - Adding orders to the orderbook
* - Cancelling orders from the orderbook
* - Modifying orders in the orderbook
* - Matching orders in the orderbook
* - Returning the number of bids, asks, and orders in the orderbook
* - Returning the orderbook level information
*
* - Supports FillOrKill orders
* - Supports Market orders
*/
class Orderbook {
public:

    OrderConfirmation addOrder(const Price price, const Quantity quantity, const Side side, const OrderType orderType) {

        OrderPtr order = new Order(getOrderId(), price, quantity, side, orderType);

        // if we can't match the FoK order, return 
        if (order->getOrderType() == OrderType::FillOrKill) {
            if (!canMatch(order->getSide(), order->getPrice())) {
                delete order; // cleanup
                return {};
            }
        }

        // if the price level doesn't exist, create it
        if (order->getSide() == Side::Buy && bidLevels.find(order->getPrice()) == bidLevels.end()) {
            Queue* q = new Queue();
            bidLevels[order->getPrice()] = q;
            bids.push({order->getPrice(), bidLevels[order->getPrice()]});
        } else if (order->getSide() == Side::Sell && askLevels.find(order->getPrice()) == askLevels.end()) {
            Queue* q = new Queue();
            askLevels[order->getPrice()] = q;
            asks.push({order->getPrice(), askLevels[order->getPrice()]});
        }

        // add the order to the price level
        QueuePtr queue = (order->getSide() == Side::Buy) ? bidLevels[order->getPrice()] : askLevels[order->getPrice()];
        queue->push_back(order);

        // store order and its location
        orders[order->getOrderId()] = order; 
        
        return OrderConfirmation{order->getOrderId(), matchOrders()};
    }

    void cancelOrder(OrderId orderId) {

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

        // if the level is empty, remove it from the heap
        if (level->empty()) {
            if (order->getSide() == Side::Buy) {
                bids.pop();
            } else {
                asks.pop();
            }
            delete level;
        }

        // delete the order
        delete order;
    }

    OrderConfirmation modifyOrder(OrderId orderId, Price price, Quantity quantity, Side side) {
            
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

    std::size_t getNumBids() const {
        return bids.size();
    }

    std::size_t getNumAsks() const {
        return asks.size();
    }

    std::size_t getNumOrders() const {
        return orders.size();
    }

    OrderBookLevelInfos getOrderInfos() const {
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
            vec.push_back(copy.top());
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

private:

    OrderId orderId = 0;
    
    // HEAPS - Allow for quick access to the best bid/ask
    // max heap based on price, then min heap based on time
    Bids bids;
    // min heap based on price, then min heap based on time
    Asks asks;

    // MAPS - Allow for quick access to the level (add, remove, modify orders)
    // map from price to level
    std::unordered_map<Price, QueuePtr> askLevels;
    // map from price to level
    std::unordered_map<Price, QueuePtr> bidLevels;

    // map of order id to order ptr
    std::unordered_map<OrderId, OrderPtr> orders;
    
    /*
    * Returns a unique order id
    */
    int getOrderId() {
        return orderId++;
    }

    /*
    * Checks if the order can be matched with the current order book 
    */
    bool canMatch(Side side, Price price) const {
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


    /*
    * Matches orders in the orderbook
    */
    Trades matchOrders() {

        Trades trades;
        trades.reserve(asks.size() + bids.size()); // At most, we can have one trade per order

        while (!bids.empty() && !asks.empty()) {

            Price bidPrice = bids.top().first;
            QueuePtr bidLevel = bids.top().second;
            auto& topBid = bidLevel->front();

            Price askPrice = asks.top().first;
            QueuePtr askLevel = asks.top().second;
            auto& topAsk = askLevel->front();

            // if the bid price is less than the ask price, we can't match
            if (bidPrice < askPrice) {
                break;
            }

            while (bids.size() && asks.size()) {

                auto tradeQuantity = std::min(topBid->getRemainingQuantity(), topAsk->getRemainingQuantity());
                topBid->fill(tradeQuantity);
                topAsk->fill(tradeQuantity);

                if (topBid->getRemainingQuantity() == 0) {
                    bidLevel->pop_front();
                    if (bidLevel->empty()) {
                        bids.pop();
                        bidLevels.erase(topBid->getPrice());
                        delete bidLevel;
                    }

                    orders.erase(topBid->getOrderId());
                    delete topBid;
                }

                if (topAsk->getRemainingQuantity() == 0) {
                    askLevel->pop_front();
                    if (askLevel->empty()) {
                        asks.pop();
                        askLevels.erase(topAsk->getPrice());
                        delete askLevel;
                    }

                    orders.erase(topAsk->getOrderId());
                    delete topAsk;
                }

                trades.push_back(
                    Trade{
                        TradeInfo{topBid->getOrderId(), topBid->getPrice(), tradeQuantity},
                        TradeInfo{topAsk->getOrderId(), topAsk->getPrice(), tradeQuantity}
                    }
                );
            }

            // if the bid is a FillOrKill order, we need to cancel it
            if (!bids.empty()) {
                auto& order = bids.top().second->front();

                if (order->getOrderType() == OrderType::FillOrKill) {
                    cancelOrder(order->getOrderId());
                }
            }

            // if the ask is a FillOrKill order, we need to cancel it
            if (!asks.empty()) {
                auto& order = asks.top().second->front();

                if (order->getOrderType() == OrderType::FillOrKill) {
                    cancelOrder(order->getOrderId());
                }
            }
            return trades;
        }
        return {};
    };
};