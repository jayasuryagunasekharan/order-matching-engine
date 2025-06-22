#include "order_book.hpp"
#include <algorithm>

void OrderBook::add_order(const Order& order) {
    if (order.side == OrderSide::Buy) {
        buy_book_[order.price].push_back(order);
    } else {
        sell_book_[order.price].push_back(order);
    }
}

bool OrderBook::cancel_order(uint64_t order_id) {
    return remove_order(order_id, buy_book_) || remove_order(order_id, sell_book_);
}

bool OrderBook::amend_order(uint64_t order_id, double new_price, uint64_t new_quantity) {
    // Try to find and amend in buy book
    for (auto it = buy_book_.begin(); it != buy_book_.end(); ++it) {
        auto& queue = it->second;
        for (auto qit = queue.begin(); qit != queue.end(); ++qit) {
            if (qit->id == order_id) {
                Order amended = *qit;
                queue.erase(qit);
                if (queue.empty()) buy_book_.erase(it);
                amended.price = new_price;
                amended.quantity = new_quantity;
                buy_book_[new_price].push_back(amended);
                return true;
            }
        }
    }
    
    // Try to find and amend in sell book
    for (auto it = sell_book_.begin(); it != sell_book_.end(); ++it) {
        auto& queue = it->second;
        for (auto qit = queue.begin(); qit != queue.end(); ++qit) {
            if (qit->id == order_id) {
                Order amended = *qit;
                queue.erase(qit);
                if (queue.empty()) sell_book_.erase(it);
                amended.price = new_price;
                amended.quantity = new_quantity;
                sell_book_[new_price].push_back(amended);
                return true;
            }
        }
    }
    return false;
}

std::vector<std::pair<Order, Order>> OrderBook::match_orders(const Order& incoming) {
    std::vector<std::pair<Order, Order>> trades;
    
    if (incoming.side == OrderSide::Buy) {
        // Matching against sell book
        double price_limit = incoming.type == OrderType::Limit ? incoming.price : std::numeric_limits<double>::max();
        uint64_t remaining = incoming.quantity;
        for (auto it = sell_book_.begin(); it != sell_book_.end() && remaining > 0;) {
            double book_price = it->first;
            if (book_price > price_limit) {
                break;
            }
            auto& queue = it->second;
            while (!queue.empty() && remaining > 0) {
                Order& resting = queue.front();
                uint64_t traded = std::min(remaining, resting.quantity);
                Order taker = incoming;
                taker.quantity = traded;
                Order maker = resting;
                maker.quantity = traded;
                trades.emplace_back(taker, maker);
                remaining -= traded;
                if (resting.quantity > traded) {
                    resting.quantity -= traded;
                } else {
                    queue.pop_front();
                }
            }
            if (queue.empty()) {
                it = sell_book_.erase(it);
            } else {
                ++it;
            }
        }
    } else {
        // Matching against buy book
        double price_limit = incoming.type == OrderType::Limit ? incoming.price : 0.0;
        uint64_t remaining = incoming.quantity;
        for (auto it = buy_book_.begin(); it != buy_book_.end() && remaining > 0;) {
            double book_price = it->first;
            if (book_price < price_limit) {
                break;
            }
            auto& queue = it->second;
            while (!queue.empty() && remaining > 0) {
                Order& resting = queue.front();
                uint64_t traded = std::min(remaining, resting.quantity);
                Order taker = incoming;
                taker.quantity = traded;
                Order maker = resting;
                maker.quantity = traded;
                trades.emplace_back(taker, maker);
                remaining -= traded;
                if (resting.quantity > traded) {
                    resting.quantity -= traded;
                } else {
                    queue.pop_front();
                }
            }
            if (queue.empty()) {
                it = buy_book_.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    return trades;
}

bool OrderBook::remove_order(uint64_t order_id, std::map<double, std::deque<Order>, std::greater<double>>& book) {
    for (auto it = book.begin(); it != book.end(); ++it) {
        auto& queue = it->second;
        for (auto qit = queue.begin(); qit != queue.end(); ++qit) {
            if (qit->id == order_id) {
                queue.erase(qit);
                if (queue.empty()) book.erase(it);
                return true;
            }
        }
    }
    return false;
}

bool OrderBook::remove_order(uint64_t order_id, std::map<double, std::deque<Order>, std::less<double>>& book) {
    for (auto it = book.begin(); it != book.end(); ++it) {
        auto& queue = it->second;
        for (auto qit = queue.begin(); qit != queue.end(); ++qit) {
            if (qit->id == order_id) {
                queue.erase(qit);
                if (queue.empty()) book.erase(it);
                return true;
            }
        }
    }
    return false;
} 