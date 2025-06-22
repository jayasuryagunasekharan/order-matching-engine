#pragma once
#include <atomic>
#include <vector>
#include <optional>

// Lock-free SPSC ring buffer queue
// Usage: LockFreeQueue<T> q(size); q.push(x); auto v = q.pop();
template<typename T>
class LockFreeQueue {
public:
    explicit LockFreeQueue(size_t size) : buffer_(size), size_(size), head_(0), tail_(0) {}

    bool push(const T& item) {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t next_head = (head + 1) % size_;
        if (next_head == tail_.load(std::memory_order_acquire)) return false; // full
        buffer_[head] = item;
        head_.store(next_head, std::memory_order_release);
        return true;
    }

    std::optional<T> pop() {
        size_t tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire)) return std::nullopt; // empty
        T item = buffer_[tail];
        tail_.store((tail + 1) % size_, std::memory_order_release);
        return item;
    }

private:
    std::vector<T> buffer_;
    const size_t size_;
    std::atomic<size_t> head_, tail_;
}; 