#include "matching_engine.hpp"
#include <chrono>
#include <sstream>

MatchingEngine::MatchingEngine(size_t queue_size, const std::string& trade_file, const std::string& stats_file)
    : logger_(trade_file, stats_file),
      order_queue_(std::make_unique<LockFreeQueue<Order>>(queue_size)),
      running_(false), matched_count_(0), order_count_(0) {}

MatchingEngine::~MatchingEngine() {
    stop();
}

void MatchingEngine::start() {
    running_ = true;
    matcher_ = std::thread(&MatchingEngine::matching_thread, this);
}

void MatchingEngine::stop() {
    running_ = false;
    if (matcher_.joinable()) matcher_.join();
}

bool MatchingEngine::submit_order(const Order& order) {
    if (!order_queue_->push(order)) {
        logger_.log_error("Order queue full, dropping order " + std::to_string(order.id));
        return false;
    }
    order_count_++;
    return true;
}

void MatchingEngine::report_stats() {
    std::ostringstream oss;
    oss << "Total orders: " << order_count_.load() << ", Matched trades: " << matched_count_.load();
    logger_.log_stats(oss.str());
}

void MatchingEngine::matching_thread() {
    while (running_) {
        auto order_opt = order_queue_->pop();
        if (!order_opt) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            continue;
        }
        auto trades = order_book_.match_orders(*order_opt);
        for (const auto& [taker, maker] : trades) {
            std::ostringstream oss;
            oss << "TRADE," << taker.id << "," << maker.id << "," 
                << (taker.side == OrderSide::Buy ? "BUY" : "SELL") << "," 
                << taker.price << "," << taker.quantity;
            logger_.log_trade(oss.str());
            matched_count_++;
        }
        // If not fully filled and is a limit order, add remainder to book
        uint64_t filled = 0;
        for (const auto& [taker, _] : trades) filled += taker.quantity;
        if (order_opt->type == OrderType::Limit && filled < order_opt->quantity) {
            Order remainder = *order_opt;
            remainder.quantity -= filled;
            order_book_.add_order(remainder);
        }
    }
} 