#include "BacktestAgent.h"

BacktestAgent::BacktestAgent(Lehmer32_t seed, Orderbook& orderbook)
    : seed(seed), orderbook(orderbook) {}

Price BacktestAgent::getBestPrice(Side side) {
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

void BacktestAgent::addOrder(Action action) {
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

void BacktestAgent::modifyOrder(const OrderId orderId, Action action) {
    orders.erase(orderId);
    orders.insert(orderbook.modifyOrder(orderId, action.price, action.quantity, action.side).first);
}

void BacktestAgent::cancelOrder(const OrderId orderId) {
    orders.erase(orderId);
    orderbook.cancelOrder(orderId);
}

void BacktestAgent::generateAction(Lehmer32_t timeStep) {
    Lehmer32_t t_seed = seed + timeStep;
    rn.setSeed(t_seed);
    int actionValue = rn.rndInt(0, 100);

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

Action BacktestAgent::generateActionDetails(ActionType actionType) {
    Action action;
    action.action = actionType;
    int canBuy = cash / getBestPrice(Side::Buy);
    int canSell = holdings;

    switch (actionType) {
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

Price BacktestAgent::getBiasedPrice(Side side) {
    Price bestPrice = getBestPrice(side);
    double bias = (side == Side::Buy ? -1 : 1) * rn.rndDouble(0, BIAS_FACTOR);

    if (rn.rndInt(1, 100) < OPPOSITE_BIAS_CHANCE) {
        bias = -bias;
    }

    return bestPrice + bias * bestPrice;
}
