CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -Iinclude

TARGET = output/OrderBook
TARGET_ST = output/StressTest

SRC_DIR = src
OUT_DIR = output

all: $(TARGET) $(TARGET_ST)

$(TARGET): $(OBJS) $(SRC_DIR)/main.cpp
	mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC_DIR)/main.cpp

$(TARGET_ST): $(OBJS) $(SRC_DIR)/stressTest.cpp
	mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET_ST) $(SRC_DIR)/stressTest.cpp

$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

stress_test: $(TARGET_ST)
	python3 python/order_generator.py
	./$(TARGET_ST)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

clean:
	rm -f $(OUT_DIR)/*.o $(TARGET) $(TARGET_ST)

.PHONY: all clean run stress_test valgrind