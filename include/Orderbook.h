#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <numeric> // Add this line
#include "Order.h"
#include "OrderLevel.h"
#include "Trade.h"
#include "OrderModify.h"
#include "OrderbookLevelInfos.h"

/*
* Orderbook class

* Architecture:
* - Orders are stored in a map from order id to order
* - Bids and Asks are stored in a max heap and min heap respectively
* - Price levels are stored in a map from price to a queue of orders
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

    Trades addOrder(OrderPtr order) {

        // if the order already exists, return 
        if (orders.find(order->getOrderId()) != orders.end()) {
            return {};
        }

        // if we can't match the FoK order, return 
        if (order->getOrderType() == OrderType::FillOrKill) {
            if (!canMatch(order->getSide(), order->getPrice())) {
                return {};
            }
        }

        // if the price level doesn't exist, create it
        if (order->getSide() == Side::Buy && bidLevels.find(order->getPrice()) == bidLevels.end()) {
            bidLevels[order->getPrice()] = Queue();
        } else if (order->getSide() == Side::Sell && askLevels.find(order->getPrice()) == askLevels.end()) {
            askLevels[order->getPrice()] = Queue();
        }

        // Add the order to the price level
        Queue& queue = (order->getSide() == Side::Buy) ? bidLevels[order->getPrice()] : askLevels[order->getPrice()];
        queue.push(order);

        // Store order and its location
        orders[order->getOrderId()] = OrderPtr{order}; 

        if (order->getSide() == Side::Buy) {
            bids.push(std::make_pair(order->getPrice(), queue));
        } else {
            asks.push(std::make_pair(order->getPrice(), queue));
        }

        return matchOrders();
    }

    void cancelOrder(OrderId orderId) {
        std :: cout << "Cancelling order " << orderId << std::endl;
        return;
    }

    Trades modifyOrder(OrderModify order) {
        std :: cout << "Modifying order " << order.getOrderId() << std::endl;
        return {};
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

        auto CreateLevelInfos = [](Price price, const Queue& q) {
            Quantity totalQuantity = 0;
            Queue copy = q; // Create a copy of the queue to iterate through it
            while (!copy.empty()) {
                totalQuantity += copy.front()->getRemainingQuantity();
                copy.pop();
            }
            return LevelInfo{price, totalQuantity};
        };

        auto HeapToVector = [](const auto& heap) {
            std::vector<std::pair<Price, Queue>> vec;
            auto copy = heap; // Create a copy of the heap to iterate through it
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
    
    // HEAPS - Allow for quick access to the best bid/ask
    // Max heap based on price, then min heap based on time
    Bids bids;
    // Min heap based on price, then min heap based on time
    Asks asks;

    // MAPS - Allow for quick access to the level (add, remove, modify orders)
    // map from price to level
    std::unordered_map<Price, Queue> askLevels;
    // map from price to level
    std::unordered_map<Price, Queue> bidLevels;

    // Map of order id to order ptr
    std::unordered_map<OrderId, OrderPtr> orders;
    

    /*
    * Checks if the order can be matched with the current order book 
    */
    bool canMatch(Side side, Price price) const {
        if (side == Side::Buy) {
            // If there are no asks, we can't match
            // If the price is less than the best ask, we can't match
            return !asks.empty() && price >= asks.top().first;
        } else {
            // If there are no bids, we can't match
            // If the price is greater than the best bid, we can't match
            return !bids.empty() && price <= bids.top().first;
        }
    }


    /*
    * Matches orders in the orderbook
    */
    Trades matchOrders() {
        Trades trades;
        trades.reserve(asks.size() + bids.size()); // At most, we can have one trade per order

        // While we can match orders
        while (!bids.empty() && !asks.empty()) {

            Price bidPrice = bids.top().first;
            Queue bidLevel = bids.top().second;
            auto& topBid = bidLevel.front();

            Price askPrice = asks.top().first;
            Queue askLevel = asks.top().second;
            auto& topAsk = askLevel.front();

            // if the bid price is less than the ask price, we can't match
            if (bidPrice < askPrice) {
                break;
            }

            while (bids.size() && asks.size()) {

                auto tradeQuantity = std::min(topBid->getRemainingQuantity(), topAsk->getRemainingQuantity());
                topBid->fill(tradeQuantity);
                topAsk->fill(tradeQuantity);

                if (topBid->getRemainingQuantity() == 0) {
                    bidLevel.pop();
                    if (bidLevel.empty()) {
                        bids.pop();
                        bidLevels.erase(topBid->getPrice());
                    }

                    orders.erase(topBid->getOrderId());
                }

                if (topAsk->getRemainingQuantity() == 0) {
                    askLevel.pop();
                    if (askLevel.empty()) {
                        asks.pop();
                        askLevels.erase(topAsk->getPrice());
                    }

                    orders.erase(topAsk->getOrderId());
                }

                // Add the trade to the list of trades
                trades.push_back(
                    Trade{
                        TradeInfo{topBid->getOrderId(), topBid->getPrice(), tradeQuantity},
                        TradeInfo{topAsk->getOrderId(), topAsk->getPrice(), tradeQuantity}
                    }
                );
            }

            // If the bid is a FillOrKill order, we need to cancel it
            if (!bids.empty()) {
                auto& order = bids.top().second.front();

                if (order->getOrderType() == OrderType::FillOrKill) {
                    cancelOrder(order->getOrderId());
                }
            }

            // If the ask is a FillOrKill order, we need to cancel it
            if (!asks.empty()) {
                auto& order = asks.top().second.front();

                if (order->getOrderType() == OrderType::FillOrKill) {
                    cancelOrder(order->getOrderId());
                }
            }
            return trades;
        }

        return {};
    };
};