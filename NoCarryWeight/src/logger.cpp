#include "logger.h"
#include "constants.h"
#include "utils.h"

#include <windows.h>
#include <iostream>
#include <iomanip>
#include <ctime>

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger() : current_level(Constants::DEFAULT_LOG_LEVEL)
{
    std::string log_file_path = getLogFilePath();
    log_file.open(log_file_path, std::ios::trunc);
    if (!log_file.is_open())
    {
        std::cerr << "Failed to open " << log_file_path << std::endl;
    }
}

Logger::~Logger()
{
    if (log_file.is_open())
    {
        log_file.close();
    }
}

void Logger::setLogLevel(LogLevel level)
{
    current_level = level;
}

std::string Logger::getTimestamp() const
{
    std::time_t now = std::time(nullptr);
    std::tm *local_time = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::getLogFilePath() const
{
    std::string plugin_directory = getPluginDirectory();

    if (!plugin_directory.empty())
    {
        return plugin_directory + Constants::getLogFilename();
    }

    return Constants::getLogFilename(); // Fallback if handle not set
}

void Logger::log(LogLevel level, const std::string &message)
{
    if (level >= current_level && log_file.is_open())
    {
        std::string level_str;
        switch (level)
        {
        case LOG_DEBUG:
            level_str = "DEBUG";
            break;
        case LOG_INFO:
            level_str = "INFO";
            break;
        case LOG_WARNING:
            level_str = "WARNING";
            break;
        case LOG_ERROR:
            level_str = "ERROR";
            break;
        }
        log_file << "[" << getTimestamp() << "] [" << level_str << "] :: " << message << std::endl;
    }
}
