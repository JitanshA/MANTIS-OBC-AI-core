#ifndef INIT_SYSTEM_H
#define INIT_SYSTEM_H

#include <memory>
#include <thread>

// Forward declare these classes to avoid needless includes:
class UART;

// If needed, include the constants:
static const int MAX_RETRIES = 5;
static const int RETRY_DELAY_MS = 2000; // 2 seconds

bool initSystem(std::unique_ptr<UART> &uart);

#endif // INIT_SYSTEM_H