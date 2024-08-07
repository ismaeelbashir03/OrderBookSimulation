/*

Orderbook - Orderbook implementation that supports:
- Fill or Kill orders
- Market orders

 */

#include <map>
#include <numeric>

#include "Types.h"
#include "Order.h"
#include "Trade.h"
#include "OrderModify.h"
#include "OrderbookLevelInfos.h"
#include <iostream>


class OrderBook {
private:
    struct OrderEntry {
        // The order itself
        OrderPtr order{nullptr};
        // The location of the order in the bids or asks list
        OrderPtrs::iterator location;
    };

    std::map<Price, OrderPtrs, std::greater<Price>> bids;
    std::map<Price, OrderPtrs, std::less<Price>> asks;
    std::unordered_map<OrderId, OrderEntry> orders;

    bool canMatch(Side side, Price price) const {
        if (side == Side::Buy) {
            // If there are no asks, we can't match
            // If the price is less than the best ask, we can't match
            return !asks.empty() && price >= asks.begin()->first;
        } else {
            // If there are no bids, we can't match
            // If the price is greater than the best bid, we can't match
            return !bids.empty() && price <= bids.begin()->first;
        }
    }

    Trades matchOrders() {
        Trades trades;
        trades.reserve(orders.size()); // At most, we can have one trade per order

        // While we can match orders
        while (!bids.empty() && !asks.empty()) {
            
            auto bidIt = bids.begin();
            auto askIt = asks.begin();


            if (bidIt != bids.end() && askIt != asks.end()) {
                auto& bidPrice = bidIt->first;
                auto& bidOrders = bidIt->second;

                auto& askPrice = askIt->first;
                auto& askOrders = askIt->second;

                // if the bid price is less than the ask price, we can't match
                if (bidPrice < askPrice) {
                    break;
                }

                while (bids.size() && asks.size()) {
                    auto& bid = bidOrders.front();
                    auto& ask = askOrders.front();

                    auto tradeQuantity = std::min(bid->getRemainingQuantity(), ask->getRemainingQuantity());
                    bid->fill(tradeQuantity);
                    ask->fill(tradeQuantity);

                    if (bid->getRemainingQuantity() == 0) {
                        bidOrders.pop_front();
                        orders.erase(bid->getOrderId()); 
                    }

                    if (ask->getRemainingQuantity() == 0) {
                        askOrders.pop_front();
                        orders.erase(ask->getOrderId());
                    }

                    if (bids.empty()) {
                        bids.erase(bidPrice);
                    }

                    if (asks.empty()) {
                        asks.erase(askPrice);
                    }

                    // Add the trade to the list of trades
                    trades.push_back(
                        Trade{
                            TradeInfo{bid->getOrderId(), bid->getPrice(), tradeQuantity},
                            TradeInfo{ask->getOrderId(), ask->getPrice(), tradeQuantity}
                        }
                    );
                }
            }

            if (!bids.empty()) {
                auto bidIt = bids.begin();

                if (bidIt != bids.end()) {
                    auto& bidOrders = bidIt->second;
                    auto& order = bidOrders.front();

                    if (order->getOrderType() == OrderType::FillAndKill) {
                        cancelOrder(order->getOrderId());
                    }
                }
            }

            if (!asks.empty()) {
                auto askIt = asks.begin();

                if (askIt != asks.end()) {
                    auto& askOrders = askIt->second;
                    auto& order = askOrders.front();

                    if (order->getOrderType() == OrderType::FillAndKill) {
                        cancelOrder(order->getOrderId());
                    }
                }
            }

            return trades;
        }

        return {};
    };

public:
    Trades addOrder(OrderPtr order) {
        if (orders.find(order->getOrderId()) != orders.end()) {
            return {};
        }

        if (order->getOrderType() == OrderType::FillAndKill) {
            if (!canMatch(order->getSide(), order->getPrice())) {
                return {};
            }
        }

        OrderPtrs::iterator iterator;

        if (order->getSide() == Side::Buy) {
            iterator = bids[order->getPrice()].insert(bids[order->getPrice()].end(), order);
        } else {
            iterator = asks[order->getPrice()].insert(asks[order->getPrice()].end(), order);
        }

        orders.insert(std::pair{order->getOrderId(), OrderEntry{order, iterator}});
        return matchOrders();
    }

    void cancelOrder(OrderId orderId) {
        if (orders.find(orderId) == orders.end()) {
            return;
        }

        auto& orderEntry = orders[orderId];
        auto& order = orderEntry.order;
        auto& location = orderEntry.location;

        if (order->getSide() == Side::Buy) {
            bids[order->getPrice()].erase(location);
            if (bids[order->getPrice()].empty()) {
                bids.erase(order->getPrice());
            }
        } else {
            asks[order->getPrice()].erase(location);
            if (asks[order->getPrice()].empty()) {
                asks.erase(order->getPrice());
            }
        }

        orders.erase(orderId);
    }

    Trades matchOrder(OrderModify order) {
        if (orders.find(order.getOrderId()) == orders.end()) {
            return {};
        }

        const auto& orderEntry = orders[order.getOrderId()];
        const auto& oldOrder = orderEntry.order;

        cancelOrder(order.getOrderId());

        return addOrder(order.toOrderPtr(oldOrder->getOrderType()));
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

        auto CreateLevelInfos = [](Price price, const OrderPtrs& orderPtrs) {
            return LevelInfo{
                price, 
                std::accumulate(orderPtrs.begin(), orderPtrs.end(), (Quantity) 0, [](Quantity total, const OrderPtr& order) {
                return total + order->getRemainingQuantity();
            })};
        };

        for (const auto& [price, orderPtrs] : bids) {
            bidInfos.push_back(CreateLevelInfos(price, orderPtrs));
        }

        for (const auto& [price, orderPtrs] : asks) {
            askInfos.push_back(CreateLevelInfos(price, orderPtrs));
        }

        return OrderBookLevelInfos{bidInfos, askInfos};
    }
};

int main() {
    OrderBook orderBook;
    const OrderId orderId = 1;
    const Price price = 100;
    const Quantity quantity = 10;
    orderBook.addOrder(std::make_shared<Order>(orderId, price, quantity, Side::Buy, OrderType::GoodTillCancel));

    std::cout << "Number of bids: " << orderBook.getNumBids() << std::endl;
    std::cout << "Number of asks: " << orderBook.getNumAsks() << std::endl;
    std::cout << "Number of orders: " << orderBook.getNumOrders() << std::endl;
    return 0;
}