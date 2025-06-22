#pragma once
#include <fstream>
#include <mutex>
#include <string>

class Logger {
public:
    Logger(const std::string& trade_file, const std::string& stats_file)
        : trade_ofs_(trade_file), stats_ofs_(stats_file) {}

    void log_trade(const std::string& line) {
        std::lock_guard<std::mutex> lock(mtx_);
        trade_ofs_ << line << '\n';
    }

    void log_stats(const std::string& line) {
        std::lock_guard<std::mutex> lock(mtx_);
        stats_ofs_ << line << '\n';
    }

    void log_error(const std::string& line) {
        std::lock_guard<std::mutex> lock(mtx_);
        // For simplicity, log errors to stats file
        stats_ofs_ << "ERROR: " << line << '\n';
    }

private:
    std::ofstream trade_ofs_;
    std::ofstream stats_ofs_;
    std::mutex mtx_;
}; 