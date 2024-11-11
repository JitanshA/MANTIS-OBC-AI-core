#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <string>
#include <mutex>

// Log file stream -> GLOBAL
extern std::ofstream logFile;
extern std::mutex logMutex;

std::string currentTimestamp();
void logMessage(const std::string &message);

#endif // UTILS_H