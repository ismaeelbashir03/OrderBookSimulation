#pragma once

#include "Types.h"
#include "OrderType.h"
#include "Side.h"
#include <memory>
#include <stdexcept>
#include <list>

class Order {
public:
    Order(OrderId orderId, Price price, Quantity quantity, Side side, OrderType orderType) {
        this->orderId = orderId;
        this->price = price;
        this->initialQuantity = quantity;
        this->remainingQuantity = quantity;
        this->side = side;
        this->orderType = orderType;
        this->time = std::chrono::system_clock::now();
    }

    OrderId getOrderId() {
        return orderId;
    }

    Price getPrice() {
        return price;
    }
    
    Side getSide() {
        return side;
    }

    Time getTime() {
        return time;
    }
    OrderType getOrderType() {
        return orderType;
    }

    Quantity getInitialQuantity() {
        return initialQuantity;
    }
    Quantity getRemainingQuantity() {
        return remainingQuantity;
    }
    Quantity getFilledQuantity() {
        return initialQuantity - remainingQuantity;
    }
    
    void fill(Quantity quantity) {
        if (quantity > remainingQuantity) {
            throw std::runtime_error("Cannot fill more than remaining quantity");
        }
        remainingQuantity -= quantity;
    }

private:
    OrderId orderId;
    Price price;
    Side side;
    OrderType orderType;
    Quantity initialQuantity;
    Quantity remainingQuantity;
    Time time;
};

using OrderPtr = Order*;
using OrderPtrs = std::list<OrderPtr>;