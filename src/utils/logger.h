
#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iostream>
#include <sstream>
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

// Convenience macros
#define LOG_INFO(message) Logger::get_instance().log(message, Logger::LogLevel::INFO)
#define LOG_WARNING(message) Logger::get_instance().log(message, Logger::LogLevel::WARNING)
#define LOG_ERROR(message) Logger::get_instance().log(message, Logger::LogLevel::ERROR)
