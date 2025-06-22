cmake_minimum_required(VERSION 3.20)
project(order_matching_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(order_matching_engine
    src/main.cpp
    src/order.hpp
    src/order_book.hpp
    src/order_book.cpp
    src/matching_engine.hpp
    src/matching_engine.cpp
    src/lockfree_queue.hpp
    src/logger.hpp
)

enable_testing()
add_subdirectory(tests) 