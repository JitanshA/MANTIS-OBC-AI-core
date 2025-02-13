#include "uartReadLoop.h"
#include "uart.h"
#include <atomic>
#include <iostream>

#define SERVER_PORT "/tmp/tty.server"

int main()
{
    std::atomic<bool> running(true);

    UART serverUart(SERVER_PORT);
    if (!serverUart.isOpen())
    {
        std::cerr << "[SERVER] Cannot open: " << SERVER_PORT << "\n";
        return -1;
    }
    std::cout << "[SERVER] Listening on " << SERVER_PORT << std::endl;

    uartReadLoop(serverUart, running);

    std::cout << "[SERVER] Exiting.\n";
    return 0;
}