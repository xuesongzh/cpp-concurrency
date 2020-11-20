/*
 *  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

#include "common/Settings.hpp"
/**
 * Double-Macro-Stringy Technique
 * This technique is being used as __FILE__,__LINE__ are predefined Macros
 * So we need to expand them twice to get the value in desired format
 */
#define LINE_NO_STR(x) #x
#define LINE_NO(x) LINE_NO_STR(x)
#define INFO telux::common::LogLevel::LEVEL_INFO
#define DEBUG telux::common::LogLevel::LEVEL_DEBUG
#define WARNING telux::common::LogLevel::LEVEL_WARNING
#define ERROR telux::common::LogLevel::LEVEL_ERROR
#define LOG(logLevel, args...) telux::common::Log().logMessage(logLevel, __FILE__, LINE_NO(__LINE__), args)
namespace telux {
namespace common {
/*
 * Logging levels supported.
 */
enum class LogLevel {
    LEVEL_NONE = 1,
    LEVEL_ERROR,
    LEVEL_WARNING,
    LEVEL_INFO,
    LEVEL_DEBUG,
};
/**
 * Logger class - A singleton class which provides interface to log messages to
 *                a console and to a log file.
 * Log level is configurable
 */
class Logger {
 public:
    static Logger &getInstance();
    /*
     * Get the current console logging level
     */
    LogLevel getConsoleLogLevel();
    /*
     * Get the file console logging level
     */
    LogLevel getFileLogLevel();
    /*
     * write log a message to console and a log file based on the settings.
     */
    void writeLogMessage(std::ostringstream &os, LogLevel logLevel);
    /*
     * is Date and Time enabled
     */
    bool isDateTimeEnabled();
    /*
     * get the current date and time of the device
     */
    const std::string getCurrentTime();
    /*
     * get the current running process name of the device
     */
    const std::string getProcessName();
    /*
     * get the current running process id.
     */
    int getProcessId();
    /*
     * Singleton implementation, copy constructors are disabled.
     */
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger();
    ~Logger();

 private:
    /*
     * initialization of logging
     */
    void init();
    /*
     * set log file max size
     */
    void setLogFileMaxSize();
    /*
     * Get Log File Max Size
     */
    int getLogFileMaxSize();
    /*
     * Get log level from given level string
     */
    LogLevel getLogLevel(std::string logLevelString);
    /*
     * set lot file name
     */
    void setLogFileName();
    /*
     * To retrieve the full name of the log file including path
     */
    std::string getLogFileName();
    /*
     * set console logging level to a desired threshold
     */
    void setConsoleLogLevel();
    /*
     * set file logging level to a desired threshold
     */
    void setFileLogLevel();
    /*
     * set data and time
     */
    void setDateTime();
    /*
     * initialize log file
     */
    void initFileLogging();
    /*
     * initialize logs to console
     */
    void initConsoleLogging();
    /*
     * write logs message to console
     */
    void writeToConsole(std::string &logMessage);
    /*
     * write log message to file
     */
    void writeToFile(std::string &logMessage);
    /*
     * set pid
     */
    void setProcessId();
    /*
     * set process name
     */
    void setProcessName();
    LogLevel consoleLogLevel_, fileLogLevel_;
    std::ofstream logFileStream_;
    std::mutex logFileMutex_;
    bool isLoggingToFileEnabled_ = false;
    bool isLoggingToConsoleEnabled_ = false;
    bool isDateTimeEnabled_ = false;
    std::string logFileFullName_;
    int logFileMaxSize_;
    int processID_;
    std::string processName_;
};
class Log {
 public:
    /**
     * Public API to log a message
     */
    template <typename... MessageArgs>
    void logMessage(LogLevel logLevel, const std::string &fileName, const std::string &lineNo, MessageArgs... params) {
        // Can add any preprocessing for logs here
        Logger &logger = Logger::getInstance();
        bool logToConsole = logger.getConsoleLogLevel() >= logLevel;
        bool logToFile = logger.getFileLogLevel() >= logLevel;
        std::string timeStamp = "";
        std::string fileNameAndLineNo = "";
        std::string processIdAndName = "";
        if (logToConsole || logToFile) {
            // Get current date and time from system if LOG_PREFIX_DATE_TIME flag enabled
            if (logger.isDateTimeEnabled()) {
                timeStamp = " " + logger.getCurrentTime();
            }
            // get process id and name
            processIdAndName = std::to_string(logger.getProcessId()) + "/" + logger.getProcessName();
            // get the filename from full path
            const char *lastSlash = std::strrchr(fileName.c_str(), '/');
            if (lastSlash != nullptr) {
                fileNameAndLineNo = " " + std::string(lastSlash + 1) + "(" + lineNo + ") ";
            } else {
                fileNameAndLineNo = " " + std::string(fileName) + "(" + lineNo + ") ";
            }
            switch (logLevel) {
                case LogLevel::LEVEL_ERROR:
                    outputStream_ << "[E]" << timeStamp << " " << processIdAndName << fileNameAndLineNo;
                    break;
                case LogLevel::LEVEL_WARNING:
                    outputStream_ << "[W]" << timeStamp << " " << processIdAndName << fileNameAndLineNo;
                    break;
                case LogLevel::LEVEL_INFO:
                    outputStream_ << "[I]" << timeStamp << " " << processIdAndName << fileNameAndLineNo;
                    break;
                case LogLevel::LEVEL_DEBUG:
                    outputStream_ << "[D]" << timeStamp << " " << processIdAndName << fileNameAndLineNo;
                    break;
                default:;
            }
            // Print thread id for debugging
            outputStream_ << std::this_thread::get_id() << ": ";
            constructMessage(params...);
            logger.writeLogMessage(outputStream_, logLevel);
        }
    }

 private:
    /*
     * Recursive helper methods to construct the complete log message
     * from input arguments
     */
    template <typename T>
    void constructMessage(T param) {
        outputStream_ << param;
    }
    template <typename T, typename... MessageArgs>
    void constructMessage(T param, MessageArgs... params) {
        outputStream_ << param;
        constructMessage(params...);
    }
    /*
     * Member variable to buffer the input log message before writing to file
     * from input arguments
     */
    std::ostringstream outputStream_;
};
}  // namespace common
}  // namespace telux
#endif