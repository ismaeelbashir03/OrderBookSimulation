#include <iostream>
#include "Orderbook.h"
#include "Order.h"
#include "OrderbookLevelInfos.h"

// global print counter
int printCounter = 0;

OrderConfirmation addOrder(Orderbook& orderbook, const Price price, const Quantity quantity, const Side side, const OrderType orderType) {
    return orderbook.addOrder(price, quantity, side, orderType);
}

OrderConfirmation modifyOrder(Orderbook& orderbook, const OrderId orderId, const Price price, const Quantity quantity, const Side side) {
    return orderbook.modifyOrder(orderId, price, quantity, side);
}

void printOrderbook(const Orderbook& orderbook) {
    OrderBookLevelInfos orderInfos = orderbook.getOrderInfos();

    std::cout << "Orderbook State: " << printCounter++ << std::endl;
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

int main() {
    Orderbook orderbook = Orderbook();

    // add Market order of 10 buy
    OrderConfirmation order1 = addOrder(orderbook, 100, 10, Side::Buy, OrderType::MarketOrder);
    printOrderbook(orderbook);

    // add Market order of 5 sell
    OrderConfirmation order2 = addOrder(orderbook, 100, 5, Side::Sell, OrderType::MarketOrder);
    printOrderbook(orderbook);

    // add Fok order of 3 buy
    OrderConfirmation order3 = addOrder(orderbook, 100, 2, Side::Sell, OrderType::FillOrKill);
    printOrderbook(orderbook);

    // add market order of 5 sell different price
    OrderConfirmation order4 = addOrder(orderbook, 110, 5, Side::Sell, OrderType::MarketOrder);
    printOrderbook(orderbook);

    // add market order of 5 buy different price
    OrderConfirmation order5 = addOrder(orderbook, 99, 5, Side::Buy, OrderType::MarketOrder);
    printOrderbook(orderbook);

    // add market order of 5 sell different price
    OrderConfirmation order6 = addOrder(orderbook, 101, 5, Side::Sell, OrderType::MarketOrder);
    printOrderbook(orderbook);

    // add market order of 5 sell different price
    OrderConfirmation order7 = addOrder(orderbook, 101, 5, Side::Sell, OrderType::MarketOrder);
    printOrderbook(orderbook);

    // cancel last order
    orderbook.cancelOrder(order7.first);
    printOrderbook(orderbook);

    // cancel order that doesnt exist
    orderbook.cancelOrder(123);
    printOrderbook(orderbook);

    // modify order to switch side
    OrderConfirmation order8 = modifyOrder(orderbook, order6.first, 101, 5, Side::Buy);
    printOrderbook(orderbook);

    return 0;
}