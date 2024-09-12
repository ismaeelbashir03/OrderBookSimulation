#pragma once
#include <unordered_set>
#include "RandomNumber.h"
#include "Orderbook.h"
#include "Order.h"

// AGENT SETTINGS
const double STARTING_CASH = 10000.00; // starting cash for agent
const int MAKE_ORDER_NO_PENDING = 50; // (%) to make order if none exist
const double ORDER_SIZE = 0.8; // max of how much of our cash to spend on each order (%)
const std::array<double, 3> PENDING_ORDER_ACTIONS = {10, 30, 60}; // (%) to cancel, modify, or add order if we have pending orders
const double NO_PRICE_OFFER = 100.00; // price to offer if no orders in orderbook (starting price)

// BIAS SETTINGS
const double BIAS_FACTOR = 0.05; // bias factor for price adjustment
const double OPPOSITE_BIAS_CHANCE = 1; // (%) chance to go in the opposite direction

// FAIR PRICE SETTINGS
const bool USE_FAIR_PRICE = true; // use fair price for order generation
const double FAIR_PRICE = 100.00; // fair price for order generation

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

    Price getBestPrice(Side side) {

        if (USE_FAIR_PRICE) {
            return FAIR_PRICE;
        }

        OrderBookLevelInfos orderInfos = orderbook.getOrderInfos();
        
        if (side == Side::Buy) {
            return orderInfos.getBids().size() == 0 ? NO_PRICE_OFFER : orderInfos.getBids().front().price;
        } else {
            return orderInfos.getAsks().size() == 0 ? NO_PRICE_OFFER : orderInfos.getAsks().front().price;
        }
    }

    void addOrder(Action action) {
        OrderConfirmation confirmation = orderbook.addOrder(action.price, action.quantity, action.side, action.orderType);
        orders.insert(confirmation.first);
        
        if (action.side == Side::Buy) {
            cash -= action.price * action.quantity;
            holdings += action.quantity;
        } else {
            cash += action.price * action.quantity;
            holdings -= action.quantity;
        }
    }

    void modifyOrder(const OrderId orderId, Action action) {
        orders.erase(orderId);
        orders.insert(orderbook.modifyOrder(orderId, action.price, action.quantity, action.side).first);
    }

    void cancelOrder(const OrderId orderId) {
        orders.erase(orderId);
        orderbook.cancelOrder(orderId);
    }

public:
    BacktestAgent(Lehmer32_t seed, Orderbook& orderbook) : seed(seed), orderbook(orderbook) {}

    void generateAction(Lehmer32_t timeStep) {
        
        // generate seed and action based on time
        Lehmer32_t t_seed = seed + timeStep;
        rn.setSeed(t_seed);
        int actionValue = rn.rndInt(0, 100);

        // if we have no orders pending
        if (orders.size() == 0) {
            if (actionValue < MAKE_ORDER_NO_PENDING) {
                addOrder(generateActionDetails(ActionType::AddOrder));
            }
        } else {
            if (actionValue < PENDING_ORDER_ACTIONS[0]) {
                cancelOrder(*orders.begin());
            } else if (actionValue < PENDING_ORDER_ACTIONS[0] + PENDING_ORDER_ACTIONS[1]) {
                modifyOrder(*orders.begin(), generateActionDetails(ActionType::ModifyOrder));
            } else {
                addOrder(generateActionDetails(ActionType::AddOrder));
            }
        }
    }

private:
    Action generateActionDetails(ActionType actionType) {
        Action action;
        action.action = actionType;
        int canBuy = cash / getBestPrice(Side::Buy);
        int canSell = holdings;
        
        switch(actionType) {
            case ActionType::AddOrder:
                action.side = static_cast<Side>(rn.rndInt(0, 1)); 
                action.price = getBiasedPrice(action.side);
                action.quantity = rn.rndInt(1, action.side == Side::Buy ? canBuy * ORDER_SIZE : canSell * ORDER_SIZE);
                action.orderType = static_cast<OrderType>(rn.rndInt(0, 1));
                break;

            case ActionType::ModifyOrder:
                action.side = static_cast<Side>(rn.rndInt(0, 1));
                action.price = getBiasedPrice(action.side);
                action.quantity = rn.rndInt(1, action.side == Side::Buy ? canBuy * ORDER_SIZE : canSell * ORDER_SIZE);
                break;
            
            case ActionType::CancelOrder:
                break;
        }
        
        return action;
    }

    Price getBiasedPrice(Side side) {
        Price bestPrice = getBestPrice(side);
        double bias = (side == Side::Buy ? -1 : 1) * rn.rndDouble(0, BIAS_FACTOR);
        
        if (rn.rndInt(1, 100) < OPPOSITE_BIAS_CHANCE) {
            bias = -bias;
        }
        
        return bestPrice + bias * bestPrice;
    }
};