#pragma once
#include "order.hpp"
#include "order_book.hpp"
#include "lockfree_queue.hpp"
#include "logger.hpp"
#include <thread>
#include <atomic>
#include <vector>
#include <memory>

class MatchingEngine {
public:
    MatchingEngine(size_t queue_size, const std::string& trade_file, const std::string& stats_file);
    ~MatchingEngine();

    void start();
    void stop();
    bool submit_order(const Order& order); // Thread-safe
    void report_stats();

private:
    void matching_thread();

    OrderBook order_book_;
    Logger logger_;
    std::unique_ptr<LockFreeQueue<Order>> order_queue_;
    std::thread matcher_;
    std::atomic<bool> running_;
    // Stats
    std::atomic<uint64_t> matched_count_;
    std::atomic<uint64_t> order_count_;
}; 