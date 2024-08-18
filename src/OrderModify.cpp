#include "OrderModify.h"

OrderModify::OrderModify(OrderId orderId, Side side, Price price, Quantity quantity)
    : orderId(orderId), side(side), price(price), quantity(quantity) {}

OrderId OrderModify::getOrderId() const {
    return orderId;
}

Quantity OrderModify::getQuantity() const {
    return quantity;
}

Price OrderModify::getPrice() const {
    return price;
}

Side OrderModify::getSide() const {
    return side;
}