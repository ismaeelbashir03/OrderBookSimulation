#include "Orderbook.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>

void testAddOrder() {
    Orderbook orderbook;
    OrderConfirmation confirmation = orderbook.addOrder(100.0, 10, Side::Buy, OrderType::LimitOrder);
    assert(orderbook.getNumOrders() == 1);
    assert(orderbook.getNumBids() == 1);
    std::cout << "testAddOrder passed.\n";
}

void testCancelOrder() {
    Orderbook orderbook;
    OrderConfirmation confirmation = orderbook.addOrder(100.0, 10, Side::Buy, OrderType::LimitOrder);
    OrderId orderId = confirmation.first;
    orderbook.cancelOrder(orderId);
    // price level should be of quantity 0, but still exist (lazy deletion)
    OrderBookLevelInfos infos = orderbook.getOrderInfos();
    assert(infos.getBids().front().quantity == 0);
    assert(orderbook.getNumOrders() == 0);
    assert(orderbook.getNumBids() == 1); // price level still exists
    std::cout << "testCancelOrder passed.\n";
}

void testModifyOrder() {
    Orderbook orderbook;
    OrderConfirmation confirmation = orderbook.addOrder(100.0, 10, Side::Buy, OrderType::LimitOrder);
    OrderId orderId = confirmation.first;
    OrderConfirmation modifiedConfirmation = orderbook.modifyOrder(orderId, 105.0, 5, Side::Buy);
    assert(orderbook.getNumOrders() == 1);
    OrderBookLevelInfos infos = orderbook.getOrderInfos();
    assert(infos.getBids().size() == 2); // price level should be of quantity 0, but still exist (lazy deletion)

    // old order should be at the back
    assert(infos.getBids().back().price == 100.0);
    assert(infos.getBids().back().quantity == 0);

    // new order should be at the front
    assert(infos.getBids().front().price == 105.0);
    assert(infos.getBids().front().quantity == 5);
    std::cout << "testModifyOrder passed.\n";
}

void testGetOrderInfos() {
    Orderbook orderbook;
    orderbook.addOrder(100.0, 10, Side::Buy, OrderType::LimitOrder);
    orderbook.addOrder(101.0, 5, Side::Buy, OrderType::LimitOrder);
    orderbook.addOrder(102.0, 20, Side::Sell, OrderType::LimitOrder);

    OrderBookLevelInfos infos = orderbook.getOrderInfos();
    assert(infos.getBids().size() == 2);
    assert(infos.getAsks().size() == 1);
    assert(infos.getBids().front().price == 101.0);
    assert(infos.getAsks().front().price == 102.0);
    std::cout << "testGetOrderInfos passed.\n";
}

void testMatchOrdersPartialFill() {
    Orderbook orderbook;
    orderbook.addOrder(100.0, 10, Side::Buy, OrderType::LimitOrder);
    orderbook.addOrder(100.0, 5, Side::Sell, OrderType::LimitOrder);
    assert(orderbook.getNumOrders() == 1);
    assert(orderbook.getNumAsks() == 0); // order fully filled
    assert(orderbook.getNumBids() == 1); // order not fully filled
    assert(orderbook.getOrderInfos().getBids().front().quantity == 5);
    std::cout << "testMatchOrders passed.\n";
}

void testMatchOrdersFullFill() {
    Orderbook orderbook;
    orderbook.addOrder(100.0, 10, Side::Buy, OrderType::LimitOrder);
    orderbook.addOrder(100.0, 10, Side::Sell, OrderType::LimitOrder);
    assert(orderbook.getNumOrders() == 0);
    assert(orderbook.getNumAsks() == 0); // order fully filled
    assert(orderbook.getNumBids() == 0); // order fully filled
    std::cout << "testMatchOrders passed.\n";
}

void createhashFile() {
    Orderbook orderbook;

    std::hash<std::string> hasher;
    std::ofstream outFile("output/hash_output.txt");

    std::ifstream file("output/hash_orders.txt");
    std::string line;
    std::string hash;

    
    while (std::getline(file, line)) {
        std::string hash;
        std::istringstream iss(line);
        int flag;
        double price;
        int quantity;
        
        if (!(iss >> flag >> price >> quantity)) {
            std::cerr << "error: " << line << std::endl;
            continue;
        }

        Side side = (flag == 1) ? Side::Buy : Side::Sell;
        
        orderbook.addOrder(price, quantity, side, OrderType::LimitOrder);

        // add the hash of the order to the hash string
        OrderBookLevelInfos infos = orderbook.getOrderInfos();
        hash += std::to_string(infos.getBids().size());
        hash += std::to_string(infos.getAsks().size());
        for (auto& bid : infos.getBids()) {
            hash += std::to_string(bid.price);
            hash += std::to_string(bid.quantity);
        }
        for (auto& ask : infos.getAsks()) {
            hash += std::to_string(ask.price);
            hash += std::to_string(ask.quantity);
        }
        size_t hashValue = hasher(hash);
        outFile << hashValue << std::endl;
    }
    outFile.close();
}

void hashTest() {
    Orderbook orderbook;

    std::hash<std::string> hasher;
    std::ifstream file("hash_orders.txt");
    std::string line;
    std::string hash;

    std::ifstream hashFile("hash_output.txt");
    std::string hashLine;

    while (std::getline(file, line)) {
        std::string hash;
        std::istringstream iss(line);
        int flag;
        double price;
        int quantity;
        
        if (!(iss >> flag >> price >> quantity)) {
            std::cerr << "error: " << line << std::endl;
            continue;
        }

        Side side = (flag == 1) ? Side::Buy : Side::Sell;
        
        orderbook.addOrder(price, quantity, side, OrderType::LimitOrder);

        // add the hash of the order to the hash string
        OrderBookLevelInfos infos = orderbook.getOrderInfos();
        hash += std::to_string(infos.getBids().size());
        hash += std::to_string(infos.getAsks().size());
        for (auto& bid : infos.getBids()) {
            hash += std::to_string(bid.price);
            hash += std::to_string(bid.quantity);
        }
        for (auto& ask : infos.getAsks()) {
            hash += std::to_string(ask.price);
            hash += std::to_string(ask.quantity);
        }
        size_t hashValue = hasher(hash);
        std::getline(hashFile, hashLine);
        size_t expectedHash = std::stoul(hashLine);
        assert(hashValue == expectedHash);
    }
    std::cout << "hashTest passed.\n";
}

int main() {
    testAddOrder();
    testCancelOrder();
    testModifyOrder();
    testGetOrderInfos();
    testMatchOrdersPartialFill();
    testMatchOrdersFullFill();
    // hash file already created, so should compare against original hash file
    // createhashFile();
    hashTest();

    std::cout << "All tests passed.\n";
    return 0;
}
