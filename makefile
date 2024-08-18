CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -Iinclude

TARGET = output/OrderBook

SRC_DIR = src
OUT_DIR = output

SRCS = $(SRC_DIR)/Order.cpp $(SRC_DIR)/Trade.cpp $(SRC_DIR)/OrderbookLevelInfos.cpp $(SRC_DIR)/OrderModify.cpp

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OUT_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS) $(SRC_DIR)/main.cpp
	mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(SRC_DIR)/main.cpp

$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

clean:
	rm -f $(OUT_DIR)/*.o $(TARGET)

.PHONY: all clean run valgrind