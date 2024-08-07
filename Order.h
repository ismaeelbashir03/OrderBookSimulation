#pragma once

#include "Types.h"
#include "OrderType.h"
#include "Side.h"
#include <memory>
#include <stdexcept>
#include <list>

class Order {
public:
    Order(OrderId orderId, Price price, Quantity quantity, Side side, OrderType orderType);

    OrderId getOrderId() const;
    Price getPrice() const;
    Side getSide() const;
    OrderType getOrderType() const;
    Quantity getInitialQuantity() const;
    Quantity getRemainingQuantity() const;
    Quantity getFilledQuantity() const;
    void fill(Quantity quantity);

private:
    OrderId orderId;
    Price price;
    Side side;
    OrderType orderType;
    Quantity initialQuantity;
    Quantity remainingQuantity;
};

using OrderPtr = std::shared_ptr<Order>;
using OrderPtrs = std::list<OrderPtr>;