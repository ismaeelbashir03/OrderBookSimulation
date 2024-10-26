CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -I include/

SRC_DIR = src
OUT_DIR = output

SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/Orderbook.cpp

# main target
build: 
	mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(OUT_DIR)/main

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
build_agent: build
	mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_DIR)/agent.cpp $(SRC_DIR)/Orderbook.cpp -o $(OUT_DIR)/agent

run_agent:
	./$(OUT_DIR)/agent

agent: build_agent run_agent

# valgrind target
valgrind_run:
	valgrind --leak-check=full ./$(OUT_DIR)/main

valgrind: build valgrind_run

# mac memory leak target
leak_run:
	ASAN_OPTIONS=detect_leaks=1 ./output/main

leak: build leak_run

clean:
	rm -f $(OUT_DIR)/*
