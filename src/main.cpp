#include "matching_engine.hpp"
#include <random>
#include <thread>
#include <iostream>
#include <atomic>

constexpr size_t QUEUE_SIZE = 1 << 16;
constexpr int DURATION_SECONDS = 5;
constexpr int ORDER_RATE = 50000; // orders/sec

std::atomic<uint64_t> global_order_id{1};

Order random_order() {
    static thread_local std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<int> side_dist(0, 1);
    std::uniform_int_distribution<int> type_dist(0, 1);
    std::uniform_real_distribution<double> price_dist(100.0, 200.0);
    std::uniform_int_distribution<uint64_t> qty_dist(1, 1000);
    Order o;
    o.id = global_order_id++;
    o.side = side_dist(rng) ? OrderSide::Buy : OrderSide::Sell;
    o.type = type_dist(rng) ? OrderType::Limit : OrderType::Market;
    o.price = price_dist(rng);
    o.quantity = qty_dist(rng);
    o.timestamp = std::chrono::high_resolution_clock::now();
    return o;
}

void order_producer(MatchingEngine& engine, std::atomic<bool>& running) {
    using namespace std::chrono;
    auto next = high_resolution_clock::now();
    while (running) {
        for (int i = 0; i < ORDER_RATE / 10; ++i) {
            engine.submit_order(random_order());
        }
        next += milliseconds(100);
        std::this_thread::sleep_until(next);
    }
}

int main() {
    MatchingEngine engine(QUEUE_SIZE, "trades.csv", "stats.txt");
    engine.start();
    std::atomic<bool> running{true};
    std::thread producer(order_producer, std::ref(engine), std::ref(running));
    std::cout << "Running simulation for " << DURATION_SECONDS << " seconds...\n";
    std::this_thread::sleep_for(std::chrono::seconds(DURATION_SECONDS));
    running = false;
    producer.join();
    engine.stop();
    engine.report_stats();
    std::cout << "Done. See trades.csv and stats.txt for output.\n";
    return 0;
} 