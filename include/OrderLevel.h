#pragma once

#include "Types.h"
#include "Order.h"
#include <queue>
#include <vector>
#include <memory>

// bid/ask types
using Queue = std::deque<OrderPtr>;
using QueuePtr = std::shared_ptr<Queue>;

struct BidComparator {
    bool operator()(const std::pair<Price, QueuePtr>& lhs,
                    const std::pair<Price, QueuePtr>& rhs) const {
        return lhs.first < rhs.first; // Max heap based on price
    }
};

struct AskComparator {
    bool operator()(const std::pair<Price, QueuePtr>& lhs,
                    const std::pair<Price, QueuePtr>& rhs) const {
        return lhs.first > rhs.first; // Min heap based on price
    }
};


using Bids = std::priority_queue<std::pair<double, QueuePtr>, std::vector<std::pair<double, QueuePtr>>, BidComparator>;
using Asks = std::priority_queue<std::pair<double, QueuePtr>, std::vector<std::pair<double, QueuePtr>>, AskComparator>;
