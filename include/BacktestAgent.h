// BacktestAgent.h
#pragma once
#include <unordered_set>
#include "RandomNumber.h"
#include "Orderbook.h"
#include "Order.h"

// AGENT SETTINGS
const double STARTING_CASH = 10000.00;
const int MAKE_ORDER_NO_PENDING = 50;
const double ORDER_SIZE = 0.8;
const std::array<double, 3> PENDING_ORDER_ACTIONS = {10, 30, 60};
const double NO_PRICE_OFFER = 100.00;

// BIAS SETTINGS
const double BIAS_FACTOR = 0.05;
const double OPPOSITE_BIAS_CHANCE = 1;

// FAIR PRICE SETTINGS
const bool USE_FAIR_PRICE = true;
const double FAIR_PRICE = 100.00;

enum class ActionType {
    AddOrder,
    CancelOrder,
    ModifyOrder
};

struct Action {
    ActionType action;
    Price price;
    Quantity quantity;
    Side side;
    OrderType orderType;
};

using PendingOrders = std::unordered_set<OrderId>;
using Cash = double;
using Holdings = int;

class BacktestAgent {
private:
    RandomNumber rn;
    Lehmer32_t seed;
    Orderbook& orderbook;
    Cash cash = STARTING_CASH;
    Holdings holdings = 0;
    PendingOrders orders;

    Price getBestPrice(Side side);
    void addOrder(Action action);
    void modifyOrder(const OrderId orderId, Action action);
    void cancelOrder(const OrderId orderId);
    Action generateActionDetails(ActionType actionType);
    Price getBiasedPrice(Side side);

public:
    BacktestAgent(Lehmer32_t seed, Orderbook& orderbook);
    void generateAction(Lehmer32_t timeStep);
};