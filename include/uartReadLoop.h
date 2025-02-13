#ifndef UART_READ_LOOP_H
#define UART_READ_LOOP_H

#include <atomic>

class UART; // Forward declaration

void uartReadLoop(UART &uart, std::atomic<bool> &running);

#endif // UART_READ_LOOP_H