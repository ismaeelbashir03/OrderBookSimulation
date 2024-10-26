# Market Simulation

## Overview

This project is a market simulation that implements an order book with agents that can trade. The agents will trade using procedurally generated data. The simulation will be extended to include various market conditions such as bullish and bearish trends.

## Features

- **Order Book**: An order book that supports adding, cancelling and modifying Limit Orders, and FoK orders with a matching algorithm.
- **Procedural Agents**: Can run simulated agent orders on exchange for any number of days.
- **Market Conditions**: (Upcoming) Simulations of different market conditions such as bullish and bearish trends.


## Todos

- [x] Implement Order Book

    - [x] Implement Limit Orders

    - [x] Implement FoK Orders

    - [x] Implement Matching Algorithm

    - [x] Implement Cancel Orders
    
    - [x] Implement Modify Orders
    
    - [x] Make sure there are no memory leaks (make valgrind)

    - [x] Stress test the order book (make stress_test)

- [-] Implement Agents 

    - [x] Implement Agents that can place orders in the market

    - [x] Implement Agents that can cancel orders in the market
    
    - [x] Implement Agents that can modify orders in the market

- [ ] Implement Market Conditions

## Installation

To get started with this project, follow these steps:

1. **Clone the repository**:
    ```sh
    git clone https://github.com/ismaeelbashir03/OrderBookSimulation.git
    cd OrderBookSimulation
    ```

2. **Install dependencies**:
    Ensure you have `g++` installed. You can install it using the following command:
    ```sh
    sudo apt-get install g++
    ```
    (Optional) Ensure you have `valgrind` installed. You can install it using the following command:
    ```sh
    sudo apt-get install valgrind
    ```

3. **Build the project and run it**:

## Usage

To run the order book (small test in main.cpp):
```sh
make orderbook
```

To run the agent simulation (simulates orders with procedural generation):
```sh
make agent
```

To run the stress test (generates 1 million random orders and tests the order book):
```sh
make stress_test
```

To run the valgrind test (checks for memory leaks):
```sh
make valgrind
```