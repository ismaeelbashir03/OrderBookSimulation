#include "Order.h"

Order::Order(OrderId orderId, Price price, Quantity quantity, Side side, OrderType orderType)
    : orderId(orderId), price(price), side(side), orderType(orderType), initialQuantity(quantity), remainingQuantity(quantity) {}

OrderId Order::getOrderId() const {
    return orderId;
}

Price Order::getPrice() const {
    return price;
}

Side Order::getSide() const {
    return side;
}

OrderType Order::getOrderType() const {
    return orderType;
}

Quantity Order::getInitialQuantity() const {
    return initialQuantity;
}

Quantity Order::getRemainingQuantity() const {
    return remainingQuantity;
}

Quantity Order::getFilledQuantity() const {
    return initialQuantity - remainingQuantity;
}

void Order::fill(Quantity quantity) {
    if (quantity > remainingQuantity) {
        throw std::runtime_error("Cannot fill more than remaining quantity");
    }
    remainingQuantity -= quantity;
}