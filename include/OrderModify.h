#pragma once

#include "Types.h"
#include "Order.h"
#include "Side.h"

class OrderModify {
public:
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity);

    OrderId getOrderId() const;
    Quantity getQuantity() const;
    Price getPrice() const;
    Side getSide() const;

private:
    OrderId orderId;
    Side side;
    Price price;
    Quantity quantity;
};