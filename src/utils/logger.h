// logger.h#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <iostream>
#include <chrono>
#include <iomanip>

class Logger {
public:
    enum class LogLevel {
        INFO,
        WARNING,
        ERROR
    };

    static Logger& get_instance();

    void log(const std::string& message, LogLevel level = LogLevel::INFO);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string get_timestamp();
    std::string log_level_to_string(LogLevel level);

    std::ofstream log_file_;
    std::mutex log_mutex_;
};