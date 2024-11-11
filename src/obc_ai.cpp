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

std::queue<command::Command> commandQueue;
std::mutex queueMutex;
std::condition_variable queueCondVar;

void processCommand();

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

void processCommand()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondVar.wait(lock, []
                          { return !commandQueue.empty(); });

        command::Command cmd = commandQueue.front();
        commandQueue.pop();
        lock.unlock();

        std::ostringstream oss;
        oss << "Processing Command: cmd: " << cmd.cmd()
            << " src: " << cmd.src()
            << " dst: " << cmd.dst();

        logMessage(oss.str());

        if (cmd.cmd() == START_ETHERNET_CMD)
        {
            logMessage("Received trigger command. Starting Ethernet listener...");

            EthernetPort ethernetPort(UDP_PORT);
            std::string receivedMessage;
            if (ethernetPort.receiveMessage(receivedMessage))
            {
                logMessage("Received text message: " + receivedMessage);
            }
            else
            {
                logMessage("Error: Failed to receive text message on Ethernet port.");
            }
        }
    }
}
