#include "order_book.hpp"
#include <cassert>
#include <iostream>

void test_add_and_match() {
    OrderBook book;
    Order buy{1, OrderSide::Buy, OrderType::Limit, 101.0, 100, std::chrono::high_resolution_clock::now(), ""};
    Order sell{2, OrderSide::Sell, OrderType::Limit, 101.0, 100, std::chrono::high_resolution_clock::now(), ""};
    book.add_order(buy);
    auto trades = book.match_orders(sell);
    assert(trades.size() == 1);
    assert(trades[0].first.id == 2 && trades[0].second.id == 1);
    std::cout << "test_add_and_match passed\n";
}

void test_cancel() {
    OrderBook book;
    Order buy{1, OrderSide::Buy, OrderType::Limit, 101.0, 100, std::chrono::high_resolution_clock::now(), ""};
    book.add_order(buy);
    assert(book.cancel_order(1));
    Order sell{2, OrderSide::Sell, OrderType::Limit, 101.0, 100, std::chrono::high_resolution_clock::now(), ""};
    auto trades = book.match_orders(sell);
    assert(trades.empty());
    std::cout << "test_cancel passed\n";
}

void test_amend() {
    OrderBook book;
    Order buy{1, OrderSide::Buy, OrderType::Limit, 101.0, 100, std::chrono::high_resolution_clock::now(), ""};
    book.add_order(buy);
    assert(book.amend_order(1, 102.0, 200));
    Order sell{2, OrderSide::Sell, OrderType::Limit, 102.0, 200, std::chrono::high_resolution_clock::now(), ""};
    auto trades = book.match_orders(sell);
    assert(trades.size() == 1);
    assert(trades[0].first.quantity == 200);
    std::cout << "test_amend passed\n";
}

int main() {
    test_add_and_match();
    test_cancel();
    test_amend();
    std::cout << "All tests passed!\n";
    return 0;
} 