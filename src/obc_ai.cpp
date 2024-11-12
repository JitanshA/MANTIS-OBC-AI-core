#include "protocol.pb.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "uart.h"
#include "ethernet.h"
#include "utils.h"
#include "constants.h"
#include "command_handling.h"
#include "uart_init.h"

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    UARTPort uart = uartInit();               // Initialize UART port and configure it.
    std::thread workerThread(processCommand); // Start a separate thread to handle command processing.
    command::Command cmd;                     // Declare a Command object to store received commands

    while (true) // Update this to have termination
    {
        if (uart.readCommand(cmd)) // Read command from UART port into the cmd object.
        {
            {
                std::lock_guard<std::mutex> lock(queueMutex); // Lock queueMutex for scope of this {} block
                commandQueue.push(cmd);                       // Add the received command to the commandQueue
            }
            queueCondVar.notify_one(); // Notify the worker thread that a new command is available
        }
        usleep(100000); // Sleep for 100 ms
    }

    workerThread.join();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
