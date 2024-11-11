#include "command_handling.h"
#include "uart.h"
#include "ethernet.h"
#include "utils.h"
#include "constants.h"
#include <sstream>
#include <string>

std::queue<command::Command> commandQueue;
std::mutex queueMutex;
std::condition_variable queueCondVar;

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