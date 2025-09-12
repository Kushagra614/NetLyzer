#ifndef NETLYZER_LOGGER_H
#define NETLYZER_LOGGER_H

#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace netlyzer {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void setLogFile(const std::string& filename);
    void setMinLevel(LogLevel level) { minLevel_ = level; }
    
    template<typename... Args>
    void log(LogLevel level, const std::string& format, Args... args) {
        if (level < minLevel_) return;
        
        std::string message = formatMessage(level, format, args...);
        writeLog(message);
    }

    // Convenience methods
    template<typename... Args> void debug(const std::string& format, Args... args) {
        log(LogLevel::DEBUG, format, args...);
    }
    
    template<typename... Args> void info(const std::string& format, Args... args) {
        log(LogLevel::INFO, format, args...);
    }
    
    template<typename... Args> void warning(const std::string& format, Args... args) {
        log(LogLevel::WARNING, format, args...);
    }
    
    template<typename... Args> void error(const std::string& format, Args... args) {
        log(LogLevel::ERROR, format, args...);
    }
    
    template<typename... Args> void critical(const std::string& format, Args... args) {
        log(LogLevel::CRITICAL, format, args...);
    }

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string getCurrentTime() const;
    std::string levelToString(LogLevel level) const;
    
    template<typename... Args>
    std::string formatMessage(LogLevel level, const std::string& format, Args... args) {
        std::ostringstream ss;
        ss << "[" << getCurrentTime() << "] "
           << "[" << levelToString(level) << "] "
           << formatString(format, args...);
        return ss.str();
    }
    
    std::string formatString(const std::string& format) { return format; }
    
    template<typename T, typename... Args>
    std::string formatString(const std::string& format, T value, Args... args) {
        size_t pos = format.find("{}");
        if (pos == std::string::npos) return format;
        
        std::ostringstream ss;
        ss << format.substr(0, pos) << value;
        return ss.str() + formatString(format.substr(pos + 2), args...);
    }
    
    void writeLog(const std::string& message);

    std::ofstream logFile_;
    LogLevel minLevel_ = LogLevel::INFO;
    std::mutex logMutex_;
};

// Global logger instance
#define LOG_DEBUG(...)    netlyzer::Logger::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...)     netlyzer::Logger::getInstance().info(__VA_ARGS__)
#define LOG_WARNING(...)  netlyzer::Logger::getInstance().warning(__VA_ARGS__)
#define LOG_ERROR(...)    netlyzer::Logger::getInstance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) netlyzer::Logger::getInstance().critical(__VA_ARGS__)

} // namespace netlyzer

#endif // NETLYZER_LOGGER_H
