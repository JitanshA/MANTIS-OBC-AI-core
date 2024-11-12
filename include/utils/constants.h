#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

// UART Configuration
constexpr int BAUD_RATE = 9600;
constexpr int BUFFER_SIZE = 1024;
constexpr char *UART_DEVICE_PATH = "/tmp/tty.receiver";

// Network Configuration
constexpr int UDP_PORT = 5001;
constexpr int ETHERNET_BUFFER_SIZE = 1024;

// Logging Configuration
constexpr const char *LOG_FILE_PATH = "log.txt";

#endif // CONSTANTS_H