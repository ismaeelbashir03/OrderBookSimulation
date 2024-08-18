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


/*
 * Testing the orderbook by spamming random orders 
 */
int main() {
    Orderbook orderbook = Orderbook();

    for (int i = 0; i < 50; i++) {
        OrderConfirmation confirmation = addOrder(orderbook, 100, 10, Side::Buy, OrderType::MarketOrder);

        if (rand() % 2 == 0) {
            orderbook.cancelOrder(confirmation.first);
        } else {
            addOrder(orderbook, 100, 10, Side::Sell, OrderType::MarketOrder);
        }
        orderbook.printOrderbook();
    }


    // add Market order of 10 buy
    OrderConfirmation order1 = addOrder(orderbook, 100, 10, Side::Buy, OrderType::MarketOrder);
    orderbook.printOrderbook();

    // add Market order of 5 sell
    OrderConfirmation order2 = addOrder(orderbook, 100, 5, Side::Sell, OrderType::MarketOrder);
    orderbook.printOrderbook();

    // add Fok order of 3 buy
    OrderConfirmation order3 = addOrder(orderbook, 100, 2, Side::Sell, OrderType::FillOrKill);
    orderbook.printOrderbook();

    // add market order of 5 sell different price
    OrderConfirmation order4 = addOrder(orderbook, 110, 5, Side::Sell, OrderType::MarketOrder);
    orderbook.printOrderbook();

    // add market order of 5 buy different price
    OrderConfirmation order5 = addOrder(orderbook, 99, 5, Side::Buy, OrderType::MarketOrder);
    orderbook.printOrderbook();

    // add market order of 5 sell different price
    OrderConfirmation order6 = addOrder(orderbook, 101, 5, Side::Sell, OrderType::MarketOrder);
    orderbook.printOrderbook();

    // add market order of 5 sell different price
    OrderConfirmation order7 = addOrder(orderbook, 101, 5, Side::Sell, OrderType::MarketOrder);
    orderbook.printOrderbook();

    // cancel last order
    orderbook.cancelOrder(order5.first);
    orderbook.printOrderbook();

    // cancel order that doesnt exist
    orderbook.cancelOrder(123);
    orderbook.printOrderbook();

    // modify order to switch side
    OrderConfirmation order8 = modifyOrder(orderbook, order6.first, 101, 5, Side::Buy);
    orderbook.printOrderbook();

    return 0;
}