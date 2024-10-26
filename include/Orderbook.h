#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <numeric>
#include "Order.h"
#include "OrderLevel.h"
#include "Trade.h"
#include "OrderbookLevelInfos.h"
#include "MemoryPool.h" 

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
    Orderbook();
    void printOrderbook();
    OrderConfirmation addOrder(const Price price, const Quantity quantity, const Side side, const OrderType orderType);
    void cancelOrder(OrderId orderId);
    OrderConfirmation modifyOrder(OrderId orderId, Price price, Quantity quantity, Side side);
    std::size_t getNumBids() const;
    std::size_t getNumAsks() const;
    std::size_t getNumOrders() const;
    OrderBookLevelInfos getOrderInfos() const;

private:
    OrderId orderId = 0;

    MemoryPool<Order> orderPool; // Memory pool to manage Order objects

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

    int getOrderId();
    bool canMatch(Side side, Price price) const;
    Trades matchOrders();
};

