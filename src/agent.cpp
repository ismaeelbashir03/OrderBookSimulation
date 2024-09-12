#include <iostream>
#include "Orderbook.h"
#include "BacktestAgent.h"

const int NUMBER_OF_AGENTS = 1000;
RandomNumber rn = RandomNumber();
const int SEED = 1234;
const int TICKS_IN_DAY = 10;

int main() {
    Orderbook orderbook = Orderbook();
    rn.setSeed(SEED);
    
    std::vector<BacktestAgent> agents;
    for (int i = 0; i < NUMBER_OF_AGENTS; i++) {
        BacktestAgent agent(rn.rndInt(1, 10000), orderbook);
        agents.push_back(agent);
    }

    std::cout << "Enter number of days to stream: " << std::endl;
    int days;
    std::cin >> days;

    for (int i = 0; i < days * TICKS_IN_DAY; i++) {
        for (auto& agent : agents) {
            agent.generateAction(i);
        }

        if (i % TICKS_IN_DAY == 0) {
            orderbook.printOrderbook();
        }
    }
    
    return 0;
}