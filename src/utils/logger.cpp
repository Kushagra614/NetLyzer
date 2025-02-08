// logger.cpp
#include "logger.h"

Logger& Logger::get_instance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    log_file_.open("netlyzer.log", std::ios::app);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
    }
}

Logger::~Logger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void Logger::log(const std::string& message, LogLevel level) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    std::string log_message = get_timestamp() + " [" + log_level_to_string(level) + "] " + message;

    // Log to console
    std::cout << log_message << std::endl;

    // Log to file
    if (log_file_.is_open()) {
        log_file_ << log_message << std::endl;
    }
}

std::string Logger::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::log_level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// Convenience macros for logging
#define LOG_INFO(message) Logger::get_instance().log(message, Logger::LogLevel::INFO)
#define LOG_WARNING(message) Logger::get_instance().log(message, Logger::LogLevel::WARNING)
#define LOG_ERROR(message) Logger::get_instance().log(message, Logger::LogLevel::ERROR)