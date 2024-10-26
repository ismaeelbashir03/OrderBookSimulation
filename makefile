CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -I include/

SRC_DIR = src
OUT_DIR = output
TEST_DIR = tests

SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/Orderbook.cpp
AGENT_SOURCES = $(SRC_DIR)/agent.cpp $(SRC_DIR)/Orderbook.cpp $(SRC_DIR)/backtestAgent.cpp

# main target
build: 
	mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(OUT_DIR)/main
	$(CXX) $(CXXFLAGS) $(AGENT_SOURCES) -o $(OUT_DIR)/agent
	$(CXX) $(CXXFLAGS) $(TEST_DIR)/OrderbookTests.cpp $(SRC_DIR)/Orderbook.cpp -o $(OUT_DIR)/OrderbookTests
	$(CXX) $(CXXFLAGS) $(TEST_DIR)/BacktestAgentTests.cpp $(SRC_DIR)/Orderbook.cpp $(SRC_DIR)/backtestAgent.cpp -o $(OUT_DIR)/BacktestAgentTests

run:
	./$(OUT_DIR)/main

orderbook: build run

# stress test target
build_stress_test:
	mkdir -p $(OUT_DIR) 
	$(CXX) $(CXXFLAGS) $(SRC_DIR)/stressTest.cpp $(SRC_DIR)/Orderbook.cpp -o $(OUT_DIR)/stress_test

run_stress_test: $(TARGET_ST)
	python3 python/order_generator.py
	./$(OUT_DIR)/stress_test

stress_test: build_stress_test run_stress_test

# agent target
run_agent:
	./$(OUT_DIR)/agent

agent: build run_agent

# valgrind target
valgrind_run:
	valgrind --leak-check=full ./$(OUT_DIR)/main

valgrind: build valgrind_run

# mac memory leak target
leak_run:
	ASAN_OPTIONS=detect_leaks=1 ./output/main

leak: build leak_run

# tests target
run_orderbook_tests:
	./$(OUT_DIR)/OrderbookTests

orderbook_tests: build run_orderbook_tests

run_backtest_agent_tests:
	./$(OUT_DIR)/BacktestAgentTests

backtest_agent_tests: build run_backtest_agent_tests

tests: build run_orderbook_tests run_backtest_agent_tests

# help target
help:
	@echo "make orderbook - build and run the project"
	@echo "make stress_test - build and run the stress test"
	@echo "make agent - build and run the agent"
	@echo "make valgrind - build and run the project with valgrind"
	@echo "make leak - build and run the project with leak check (mac)"
	@echo "make orderbook_tests - build and run the orderbook tests"
	@echo "make backtest_agent_tests - build and run the backtest agent tests"
	@echo "make tests - build and run all tests"
	@echo "make clean - remove all output files"

clean:
	rm -f $(OUT_DIR)/*
