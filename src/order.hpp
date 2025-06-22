#pragma once
#include <cstdint>
#include <string>
#include <chrono>

enum class OrderSide { Buy, Sell };
enum class OrderType { Limit, Market };

struct Order {
    uint64_t id;
    OrderSide side;
    OrderType type;
    double price; // For market orders, price can be ignored
    uint64_t quantity;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string user_data; // Optional, for extensibility
}; 