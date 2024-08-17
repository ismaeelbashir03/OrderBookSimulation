#include <iostream>
#include "Orderbook.h"
#include "Order.h"
#include "OrderbookLevelInfos.h"

int main() {
    Orderbook orderbook = Orderbook();

    const OrderId orderId = 1;
    const Price price = 100;
    const Quantity quantity = 10;
    orderbook.addOrder(std::make_shared<Order>(orderId, price, quantity, Side::Buy, OrderType::MarketOrder));

    OrderBookLevelInfos orderInfos = orderbook.getOrderInfos();

    std::cout << "Bid Levels:" << std::endl;
    for (const auto& level : orderInfos.getBids()) {
        std::cout << "Price: " << level.price << ", Quantity: " << level.quantity << std::endl;
    }

    std::cout << "Ask Levels:" << std::endl;
    for (const auto& level : orderInfos.getAsks()) {
        std::cout << "Price: " << level.price << ", Quantity: " << level.quantity << std::endl;
    }

    return 0;
}