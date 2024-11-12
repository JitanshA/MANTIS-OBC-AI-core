#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger
{
public:
    // Initialize the logger with log path
    static void initialize(const std::string &logFilePath);

    // Log a message with INFO level
    static void log(const std::string &message);

    // Log a message with ERROR level
    static void logError(const std::string &message);

private:
    static std::ofstream logFileStream; // File stream for logging to a file
    static std::mutex logMutex;         // Mutex for thread-safe logging

    // Helper function to get the current timestamp as a string
    static std::string getCurrentTimestamp();

    // Internal function to log messages with a specific level
    static void logInternal(const std::string &message, const std::string &level);
};

#endif // LOGGER_H