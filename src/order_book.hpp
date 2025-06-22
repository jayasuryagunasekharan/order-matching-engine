#pragma once
#include "order.hpp"
#include <map>
#include <deque>
#include <optional>
#include <vector>

class OrderBook {
public:
    // Add a new order to the book
    void add_order(const Order& order);
    // Cancel an order by id
    bool cancel_order(uint64_t order_id);
    // Amend an order's quantity (and/or price for limit orders)
    bool amend_order(uint64_t order_id, double new_price, uint64_t new_quantity);
    // Match incoming order, return trades (as pairs of taker/maker orders)
    std::vector<std::pair<Order, Order>> match_orders(const Order& incoming);

private:
    // Buy: highest price first, Sell: lowest price first
    std::map<double, std::deque<Order>, std::greater<double>> buy_book_;
    std::map<double, std::deque<Order>, std::less<double>> sell_book_;
    // Helper to find and remove order by id
    bool remove_order(uint64_t order_id, std::map<double, std::deque<Order>, std::greater<double>>& book);
    bool remove_order(uint64_t order_id, std::map<double, std::deque<Order>, std::less<double>>& book);
}; 