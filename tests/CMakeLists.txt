add_executable(order_book_test order_book_test.cpp
    ${CMAKE_SOURCE_DIR}/src/order_book.cpp
    ${CMAKE_SOURCE_DIR}/src/matching_engine.cpp)
target_include_directories(order_book_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME OrderBookTest COMMAND order_book_test) 