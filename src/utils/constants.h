#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

// UART Configuration
constexpr int BAUD_RATE = 9600;
constexpr int BUFFER_SIZE = 1024;
constexpr char OBC_AI_DEV[] = "/tmp/tty.receiver";

// Network Configuration
constexpr int UDP_PORT = 5001;
constexpr int ETHERNET_BUFFER_SIZE = 1024;
constexpr char ETHERNET_IP[] = "127.0.0.1";

// Logging Configuration
constexpr char LOG_FILE_PATH[] = "../log.txt";
constexpr char IMAGE_OUTPUT_FILE[] = "received_image.jpg";

// Command Identifiers
constexpr int START_ETHERNET_CMD = 100;

#endif // CONSTANTS_H