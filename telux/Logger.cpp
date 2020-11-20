/*
 *  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/**
 * Logger class - provides wrappers for writing log messages to console and log
 * file
 */
extern "C" {
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
}
#include "EnvUtils.hpp"
#include "common/Logger.hpp"
#include "common/Settings.hpp"
#define STAT_FAILURE -1
#define DEFAULT_LOG_FILE_PATH "./"
#define DEFAULT_LOG_FILE_NAME "tel.log"
#define DEFAULT_LOG_FILE_MAX_SIZE 5 * 1024 * 1024  // 5 MB
namespace telux {
namespace common {
Logger &Logger::getInstance() {
    static Logger instance;
    return instance;
}
Logger::Logger() {
    init();
}
Logger::~Logger() {
    // Close log file stream if it is open
    if (logFileStream_.is_open()) {
        logFileStream_.close();
    }
}
void Logger::setProcessId() {
    processID_ = getpid();
}
void Logger::setProcessName() {
    processName_ = EnvUtils::getCurrentAppName();
}
int Logger::getProcessId() {
    return processID_;
}
const std::string Logger::getProcessName() {
    return processName_;
}
void Logger::init() {
    // set required data member by reading configuration file
    setFileLogLevel();
    setConsoleLogLevel();
    setLogFileName();
    setLogFileMaxSize();
    setDateTime();
    setProcessName();
    setProcessId();
    // initialize logging
    initFileLogging();
    initConsoleLogging();
}
void Logger::initFileLogging() {
    if (fileLogLevel_ == LogLevel::LEVEL_NONE) {
        isLoggingToFileEnabled_ = false;
    }
    isLoggingToFileEnabled_ = true;
    // Initialize the log file
    if (!logFileStream_.is_open()) {
        // Open the log file for writing
        std::string logFile = getLogFileName();
        logFileStream_.open(logFile, std::ios::app);
    }
}
void Logger::initConsoleLogging() {
    if (consoleLogLevel_ == LogLevel::LEVEL_NONE) {
        isLoggingToConsoleEnabled_ = false;
    }
    isLoggingToConsoleEnabled_ = true;
}
void Logger::setLogFileName() {
    logFileFullName_ = Settings::getValue(std::string("LOG_FILE_PATH"));
    // Use current directory as default path
    if (logFileFullName_ == "") {
        logFileFullName_ += DEFAULT_LOG_FILE_PATH;
    }
    logFileFullName_ += "/";
    std::string logFileName = Settings::getValue(std::string("LOG_FILE_NAME"));
    if (logFileName != "") {
        logFileFullName_ += logFileName;
    } else {
        logFileFullName_ += DEFAULT_LOG_FILE_NAME;  // Default log file name if not configured
    }
}
std::string Logger::getLogFileName() {
    return logFileFullName_;
}
void Logger::setLogFileMaxSize() {
    std::string val = Settings::getValue(std::string("MAX_LOG_FILE_SIZE"));
    logFileMaxSize_ = DEFAULT_LOG_FILE_MAX_SIZE;
    if (!val.empty()) {
        logFileMaxSize_ = stoi(val);
    }
}
int Logger::getLogFileMaxSize() {
    return logFileMaxSize_;
}
bool Logger::isDateTimeEnabled() {
    return isDateTimeEnabled_;
}
const std::string Logger::getCurrentTime() {
    std::tm tmSnapshot;
    std::stringstream ss;
    // Get the current calendar time (Epoch time)
    std::time_t nowTime = std::time(nullptr);
    // std::put_time to get the date and time information from a given calendar time.
    // ::localtime_r() converts the calendar time to broken-down time representation
    // but stores the data in a user-supplied struct, and it is thread safe
    ss << std::put_time(::localtime_r(&nowTime, &tmSnapshot), "%b %d %H:%M:%S %Y");
    return ss.str();
}
void Logger::setDateTime() {
    std::string val = Settings::getValue(std::string("LOG_PREFIX_DATE_TIME"));
    if (!val.empty()) {
        if (val == "TRUE") {
            isDateTimeEnabled_ = true;
        } else if (val == "FALSE") {
            isDateTimeEnabled_ = false;
        }
    }
}
void Logger::setConsoleLogLevel() {
    consoleLogLevel_ = LogLevel::LEVEL_INFO;
    std::string logLevelString = Settings::getValue(std::string("CONSOLE_LOG_LEVEL"));
    if (!logLevelString.empty()) {
        consoleLogLevel_ = getLogLevel(logLevelString);
    }
}
LogLevel Logger::getConsoleLogLevel() {
    return consoleLogLevel_;
}
void Logger::setFileLogLevel() {
    fileLogLevel_ = LogLevel::LEVEL_INFO;
    std::string logLevelString = Settings::getValue(std::string("FILE_LOG_LEVEL"));
    if (!logLevelString.empty()) {
        fileLogLevel_ = getLogLevel(logLevelString);
    }
}
LogLevel Logger::getFileLogLevel() {
    return fileLogLevel_;
}
void Logger::writeToConsole(std::string &logMessage) {
    // newline applied will flush the buffer to stdout
    std::cout << logMessage << std::endl;
}
void Logger::writeToFile(std::string &logMessage) {
    // Protects log file stream when it is accessed by multiple threads
    std::lock_guard<std::mutex> lock(logFileMutex_);
    if (logFileStream_.rdstate() == std::ios_base::goodbit) {
        struct stat st;
        stat(logFileFullName_.c_str(), &st);
        // check for the size of the file
        if (st.st_size > logFileMaxSize_) {
            logFileStream_.close();
            // if backup file exists on the disk delete it
            std::string backupFileName = logFileFullName_ + ".backup";
            if (stat(backupFileName.c_str(), &st) != STAT_FAILURE) {
                remove(backupFileName.c_str());
            }
            // backup current log file
            rename(logFileFullName_.c_str(), backupFileName.c_str());
            // open new log file
            logFileStream_.open(logFileFullName_, std::ios::app);
        }
        // Write the log message into the file
        logFileStream_ << logMessage << std::endl;
    }
}
void Logger::writeLogMessage(std::ostringstream &os, LogLevel logLevel) {
    if (!os.str().empty()) {
        std::string logStream = os.str();
        if (getConsoleLogLevel() >= logLevel) {
            writeToConsole(logStream);
        }
        // Don't log into file unless logging to file is enabled
        if (getFileLogLevel() >= logLevel) {
            writeToFile(logStream);
        }
    }
}
LogLevel Logger::getLogLevel(std::string logLevelString) {
    LogLevel logLevel = LogLevel::LEVEL_DEBUG;  // default log level
    if (logLevelString == "ERROR") {
        logLevel = LogLevel::LEVEL_ERROR;
    } else if (logLevelString == "WARNING") {
        logLevel = LogLevel::LEVEL_WARNING;
    } else if (logLevelString == "INFO") {
        logLevel = LogLevel::LEVEL_INFO;
    } else if (logLevelString == "NONE") {
        logLevel = LogLevel::LEVEL_NONE;
    }
    return logLevel;
}
}  // namespace common
}  // namespace telux