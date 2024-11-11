#include "utils.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include "constants.h"

// Initialize log file stream
std::ofstream logFile(LOG_FILE_PATH, std::ios::app);
std::mutex logMutex;

std::string currentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void logMessage(const std::string &message)
{
    std::lock_guard<std::mutex> lock(logMutex); // Ensure thread-safe access to log file
    logFile << "[" << currentTimestamp() << "] " << message << std::endl;
}