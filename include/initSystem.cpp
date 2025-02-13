#include "initSystem.h"
#include "uart.h"
#include <iostream>
#include <chrono>
#include <thread>

#define SERVER_PORT "/tmp/tty.server"

bool initSystem(std::unique_ptr<UART> &uart)
{
    int attempts = 0;

    while (attempts < MAX_RETRIES)
    {
        try
        {
            // Initialize UART
            uart = std::make_unique<UART>(SERVER_PORT);
            std::cout << "[SUCCESS] UART initialized on port " << SERVER_PORT << "\n";
            return true; // Everything set up
        }
        catch (const UARTException &e)
        {
            std::cerr << "[ERROR] UART initialization failed: " << e.what() << "\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << "[FATAL] Unexpected error during initialization: " << e.what() << "\n";
        }

        attempts++;
        if (attempts >= MAX_RETRIES)
        {
            std::cerr << "[FATAL] Unable to initialize after " << MAX_RETRIES << " attempts. Exiting.\n";
            return false;
        }

        std::cerr << "[RETRY] Retrying in " << (RETRY_DELAY_MS / 1000) << " seconds...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
    }

    return false;
}