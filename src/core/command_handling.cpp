#include "command_handling.h"
#include "uart.h"
#include "ethernet.h"
#include "utils.h"
#include "constants.h"
#include <future>
#include <sstream>
#include <string>

std::queue<command::Command> commandQueue; // Shared queue to store commands and fetch commands from
std::mutex queueMutex;                     // Mutex for managing queue access
std::condition_variable queueCondVar;      // Notifies when commands are added

std::string setGpioHigh()
{
    logMessage("Setting GPIO pin to HIGH...");
    logMessage("GPIO pin set to HIGH successfully.");
    return "GPIO set to HIGH";
}

std::string setGpioLow()
{
    logMessage("Setting GPIO pin to LOW...");
    logMessage("GPIO pin set to LOW successfully.");
    return "GPIO set to LOW";
}

void processCommand()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(queueMutex); // Lock queueMutex

        // Wait until there is at least one command in the queue
        // The lambda function [] { return !commandQueue.empty(); }
        // returns true when queue is not empty, at which point
        // the thread proceeds
        queueCondVar.wait(lock, []
                          { return !commandQueue.empty(); });

        command::Command cmd = commandQueue.front(); // Fetch the next command from the queue
        commandQueue.pop();                          // Remove the processed command from the queue
        lock.unlock();                               // Unlock the mutex to allow other threads access to queue

        std::ostringstream oss; // Build a log message
        oss << "Processing Command:\n"
            << " - cmd: " << cmd.cmd() << "\n"
            << " - src: " << cmd.src() << "\n"
            << " - dst: " << cmd.dst() << "\n"
            << " - dtt: " << cmd.dtt() << "\n"
            << " - sig: " << cmd.sig() << "\n";

        if (cmd.has_string_d())
            oss << " - data (string): " << cmd.string_d() << "\n";
        else if (cmd.has_bytes_d())
            oss << " - data (bytes): " << std::string(cmd.bytes_d().begin(), cmd.bytes_d().end()) << "\n";
        else if (cmd.has_int_d())
            oss << " - data (int): " << cmd.int_d() << "\n";
        else if (cmd.has_double_d())
            oss << " - data (double): " << cmd.double_d() << "\n";
        else
            oss << " - data: None\n";

        logMessage(oss.str());

        switch (cmd.cmd())
        {
        case 1:
            logMessage("Handling command 1.");
            break;

        case 3:
            logMessage("Handling command 3: Set GPIO pin to HIGH.");
            {
                std::future<std::string> gpioHighFuture = std::async(std::launch::async, setGpioHigh);

                logMessage("Waiting for GPIO HIGH task to complete...");
                std::string result = gpioHighFuture.get();
                logMessage("GPIO HIGH Task Result: " + result);
            }
            break;

        case 4:
            logMessage("Handling command 4: Set GPIO pin to LOW.");
            {
                std::future<std::string> gpioLowFuture = std::async(std::launch::async, setGpioLow);

                logMessage("Waiting for GPIO LOW task to complete...");
                std::string result = gpioLowFuture.get();
                logMessage("GPIO LOW Task Result: " + result);
            }
            break;

        default:
            logMessage("Unknown command received.");
            break;
        }
    }
}