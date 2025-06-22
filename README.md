# Order Matching Engine

A high-performance, multithreaded C++23 order matching engine simulating a real-time stock exchange. Supports limit and market orders with price-time priority, optimized for multicore systems.

## Features
- Limit and market order support
- Price-time priority matching
- Multithreaded (producer-consumer, lock-free queues)
- Efficient order book (price-to-queue mapping)
- Dummy/random order generation for simulation
- File output for trades and performance stats
- Unit and stress tests

## Build Instructions

1. **Install CMake and a C++23 compiler (e.g., GCC 13+, Clang 16+, MSVC 2022+).**
2. Clone this repository.
3. Build:
   ```sh
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```
4. Run:
   ```sh
   ./order_matching_engine
   ```

## Usage
- Trades and stats are written to output files in the project directory.
- Modify simulation parameters in `main.cpp` as needed.

## Project Structure
- `src/` - Core engine and data structures
- `tests/` - Unit and stress tests
- `CMakeLists.txt` - Build configuration

## License
MIT 