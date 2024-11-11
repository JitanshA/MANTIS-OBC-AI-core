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

    UARTPort uart = uartInit();

    std::thread workerThread(processCommand);

    command::Command cmd;

    while (true) // Update this to have termination
    {
        if (uart.readCommand(cmd))
        {
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                commandQueue.push(cmd);
            }
            queueCondVar.notify_one();
        }
        usleep(100000);
    }

    workerThread.join();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
