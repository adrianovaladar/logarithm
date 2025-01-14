#ifndef LOGORITHM_LOGGER_H
#define LOGORITHM_LOGGER_H

#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <source_location>
#include <string>
/**
 * @brief Enumeration for log levels.
 */
enum class LOGLEVEL : char {
    Info = 'I',    /**< Informational message. */
    Warning = 'W', /**< Warning message. */
    Error = 'E'    /**< Error message. */
};

/**
 * @brief Meyer’s Singleton class for logging messages.
 */
class Logger {
public:
    /**
     * @brief Get the singleton instance of Logger.
     * @return Reference to the Logger instance.
     */
    static Logger &getInstance() {
        static Logger instance;// Static local variable ensures lazy initialization and thread safety
        return instance;
    }
    Logger(const Logger &) = delete;           // Disable copy constructor
    Logger &operator=(const Logger &) = delete;// Disable copy assignment operator
    /**
      * @brief Log a message.
      * @param text The text of the log message.
      * @param level The log level.
      * @param source The source location of the log message.
     */
    void log(const std::string &text, LOGLEVEL level, std::source_location source = std::source_location::current());
    /**
     * @brief Get the file name of the log file.
     * @return The file name of the log file.
     */
    const std::filesystem::path &getLogFileName() const;

private:
    std::mutex mutex {};/**< Mutex for thread safety. */
    std::filesystem::path logFileName;/**< File name of the log file. */
    std::atomic<bool> errorReported{false};/**< Atomic boolean flag indicating whether an error has been reported. */
    std::ofstream file;/**< Output file stream for logging. */
    /**
     * @brief Private constructor to prevent instantiation from outside.
     */
    Logger() {
        std::filesystem::path directoryName = "logs";
        if (!exists(directoryName) && !is_directory(directoryName)) {
            create_directory(directoryName);
        }
        logFileName += directoryName / "log_";
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm{};
        localtime_r(&nowTime, &now_tm);
        std::stringstream ss;
        ss << std::put_time(&now_tm, "%Y-%m-%d");
        logFileName += ss.str() + ".txt";
        file.open(logFileName, std::ofstream::app);
        if (!file.is_open()) {
            std::cerr << "Failed to open log file: " << logFileName << std::endl;
            errorReported.exchange(true);
        }
    }
    /**
     * @brief Destructor to close the log file.
     */
    virtual ~Logger() = default;
};
/**
 * @brief Global logger instance.
 *
 * This global variable provides access to the singleton instance of the Logger class.
 * It can be used throughout the code.
 */
inline Logger &logger = Logger::getInstance();


#endif//LOGORITHM_LOGGER_H
