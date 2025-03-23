#include "uartReadLoop.h"
#include "uart.h"
#include "messageIO.h"

#include <sys/select.h>
#include <atomic>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <unistd.h>

void uartReadLoop(UART &uart, std::atomic<bool> &running)
{
    while (running)
    {
        try
        {
            fd_set read_fds;
            FD_ZERO(&read_fds);
            FD_SET(uart.getFd(), &read_fds);

            int activity = select(uart.getFd() + 1, &read_fds, nullptr, nullptr, nullptr);
            if (activity < 0)
            {
                if (errno == EINTR)
                    continue; // Interrupted by a signal
                throw UARTException("select() failed: " + std::string(strerror(errno)));
            }
            else if (activity == 0)
            {
                continue;
            }

            if (FD_ISSET(uart.getFd(), &read_fds))
            {
                auto msgOpt = readLengthPrefixedMessage(uart, 4096);
                if (!msgOpt.has_value())
                {
                    continue;
                }

                parseAndPrintCommand(msgOpt.value());
            }
        }
        catch (const UARTException &e)
        {
            std::cerr << "[ERROR] UART read error: " << e.what() << std::endl;
            running = false;
        }
        catch (const std::exception &e)
        {
            std::cerr << "[FATAL] Unexpected error: " << e.what() << std::endl;
            running = false;
        }
    }

    std::cout << "[UART] Read loop exiting..." << std::endl;
}
