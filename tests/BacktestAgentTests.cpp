#include <iostream>
#include <vector>
#include <string>
#include "Orderbook.h"
#include "BacktestAgent.h"
#include "RandomNumber.h"
#include <cassert>

const int NUMBER_OF_AGENTS = 1000;
const int SEED = 1234;
const int TICKS_IN_DAY = 100;
const int TEST_DAYS = 2;

void testBacktestAgentSeedingConsistent() {
    RandomNumber rn1 = RandomNumber();
    rn1.setSeed(SEED);

    Orderbook orderbook1 = Orderbook();
    Orderbook orderbook2 = Orderbook();

    std::vector<BacktestAgent> agents1;
    std::vector<BacktestAgent> agents2;
    
    for (int i = 0; i < NUMBER_OF_AGENTS; i++) {
        BacktestAgent agent1 = BacktestAgent(rn1.rndInt(1, 10000), orderbook1);
        BacktestAgent agent2 = BacktestAgent(rn1.rndInt(1, 10000), orderbook2);
        agents1.push_back(agent1);
        agents2.push_back(agent2);
    }

    // run both sets of agents for the same number of ticks and compare orderbook states
    for (int i = 0; i < TEST_DAYS * TICKS_IN_DAY; i++) {
        for (auto& agent : agents1) {
            agent.generateAction(i);
        }
        for (auto& agent : agents2) {
            agent.generateAction(i);
        }

        if (i % TICKS_IN_DAY == 0) {
            OrderBookLevelInfos state1 = orderbook1.getOrderInfos();
            OrderBookLevelInfos state2 = orderbook2.getOrderInfos();
            
            LevelInfos bids1 = state1.getBids();
            LevelInfos bids2 = state2.getBids();
            LevelInfos asks1 = state1.getAsks();
            LevelInfos asks2 = state2.getAsks();

            assert(bids1.size() == bids2.size() && "Bids size should match for the same seed");
            assert(asks1.size() == asks2.size() && "Asks size should match for the same seed");

            for (size_t j = 0; j < bids1.size(); ++j) {
                assert(bids1[j].price == bids2[j].price && "Bid prices should match for the same seed");
            }

            for (size_t j = 0; j < asks1.size(); ++j) {
                assert(asks1[j].price == asks2[j].price && "Ask prices should match for the same seed");
            }
        }
    }

    std::cout << "testBacktestAgentSeedingConsistent passed.\n";
}

int main() {
    testBacktestAgentSeedingConsistent();
    std::cout << "All tests passed.\n";
    return 0;
}
