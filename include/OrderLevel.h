#pragma once

#include "Types.h"
#include "Order.h"
#include <queue>
#include <vector>
#include <memory>

// bid/ask types
using Queue = std::queue<OrderPtr>;


struct BidComparator {
    bool operator()(const std::pair<Price, Queue>& lhs,
                    const std::pair<Price, Queue>& rhs) const {
        return lhs.first < rhs.first; // Max heap based on price
    }
};

struct AskComparator {
    bool operator()(const std::pair<Price, Queue>& lhs,
                    const std::pair<Price, Queue>& rhs) const {
        return lhs.first > rhs.first; // Min heap based on price
    }
};


using Bids = std::priority_queue<std::pair<double, Queue>, std::vector<std::pair<double, Queue>>, BidComparator>;
using Asks = std::priority_queue<std::pair<double, Queue>, std::vector<std::pair<double, Queue>>, AskComparator>;
