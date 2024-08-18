#include <iostream>
#include <fstream> 
#include <sstream> 
#include "Orderbook.h"
#include "Order.h"
#include "OrderbookLevelInfos.h"

/*
 * Stress test the Orderbook class for benchmarking 
 */
int main() {

    Orderbook orderbook;

    std::ifstream infile("output/orders.txt");
    std::string line;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        int flag;
        double price;
        int quantity;
        
        if (!(iss >> flag >> price >> quantity)) {
            std::cerr << "error: " << line << std::endl;
            continue;
        }

        Side side = (flag == 1) ? Side::Buy : Side::Sell;
        
        orderbook.addOrder(price, quantity, side, OrderType::MarketOrder);
    }

    infile.close();
    orderbook.printOrderbook();
    return 0;

}