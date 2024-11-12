#include "utils/logger.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>

std::ofstream Logger::logFileStream;
std::mutex Logger::logMutex;

void Logger::initialize(const std::string &logFilePath)
{
    std::lock_guard<std::mutex> lock(logMutex);
    logFileStream.open(logFilePath, std::ios::app);
    if (!logFileStream)
    {
        std::cerr << "Error opening log file: " << logFilePath << std::endl;
    }
}

void Logger::log(const std::string &message)
{
    logInternal(message, "INFO");
}

void Logger::logError(const std::string &message)
{
    logInternal(message, "ERROR");
}

std::string Logger::getCurrentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void Logger::logInternal(const std::string &message, const std::string &level)
{
    std::lock_guard<std::mutex> lock(logMutex); // Ensure thread safety

    std::string timestamp = getCurrentTimestamp();
    std::string logEntry = "[" + timestamp + "] [" + level + "] " + message;

    std::cout << logEntry << std::endl;

    if (logFileStream)
    {
        logFileStream << logEntry << std::endl;
    }
}