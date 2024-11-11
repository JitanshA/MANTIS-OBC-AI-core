#include "protocol.pb.h"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "uart.h"
#include "ethernet.h"
#include "utils.h"
#include "constants.h"
#include "command_handling.h"

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    UARTPort uart("/tmp/tty.receiver");

    if (!uart.configurePort())
    {
        return 1;
    }

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
