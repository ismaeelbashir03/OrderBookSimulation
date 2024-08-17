# Market Simulation

## Overview

This project is a market simulation that implements an order book with agents that can trade. The simulation will be extended to include various market conditions such as bullish and bearish trends.

## Features

- **Order Book** (In progress): An order book that supports adding, cancelling and modifying Market Orders, and FoK orders with a matching algorithm.
- **Agents**: (Upcoming) Agents that can place and cancel orders in the market.
- **Market Conditions**: (Upcoming) Simulations of different market conditions such as bullish and bearish trends.


## Todos

- [ ] Implement Order Book

    - [x] Implement Market Orders

    - [x] Implement FoK Orders

    - [x] Implement Matching Algorithm

    - [ ] Implement Cancel Orders
    
    - [ ] Implement Modify Orders
    
    - [ ] Make sure there are no memory leaks

- [ ] Implement Agents
- [ ] Implement Market Conditions

## Installation

To get started with this project, follow these steps:

1. **Clone the repository**:
    ```sh
    git clone https://github.com/ismaeelbashir03/OrderBookSimulation.git
    cd OrderBookSimulation
    ```

2. **Build the project**:
    Ensure you have `g++` installed. You can use the provided Makefile to build the project.
    ```sh
    make
    ```

## Usage

After building the project, you can run the simulation:

```sh
./output/OrderBook
