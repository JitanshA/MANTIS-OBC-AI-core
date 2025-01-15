#include <fcntl.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "protocol.pb.h"
#include "uart.hpp"

#define SERVER_PORT "/tmp/tty.server"
#define BUFFER_SIZE 256

std::atomic<bool> emergencyStop(false);
std::atomic<bool> taskRunning(false);
std::thread actionThread;
std::mutex taskMutex;
std::condition_variable taskCondition;

void listenUART(int fd);
void parseAndExecuteCommand(const std::string& receivedData);
void performLongRunningTask();
void stopTask();

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    std::string serverPort = "/tmp/tty.server";
    int fd = openPort(serverPort);
    if (fd < 0) {
        std::cerr << "Failed to open port.\n";
        return EXIT_FAILURE;
    }

    listenUART(fd);

    close(fd);
    if (actionThread.joinable()) {
        actionThread.join();
    }
    google::protobuf::ShutdownProtobufLibrary();
    return EXIT_SUCCESS;
}

void listenUART(int fd) {
    char buffer[BUFFER_SIZE];

    while (!emergencyStop) {
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            std::string receivedData(buffer, bytesRead);
            parseAndExecuteCommand(receivedData);
        } else if (bytesRead < 0) {
            perror("Error reading from serial port");
        }
    }
}

void parseAndExecuteCommand(const std::string& receivedData) {
    command::Command msg;

    if (msg.ParseFromString(receivedData)) {
        std::cout << "\nReceived message:\n";
        std::cout << "Command: " << msg.cmd() << "\n";

        switch (msg.cmd()) {
            case 99:
                std::cout << "Emergency stop command received!\n";
                stopTask();
                break;

            case 1: {
                std::lock_guard<std::mutex> lock(taskMutex);
                if (!taskRunning) {
                    std::cout << "Starting long-running task.\n";
                    taskRunning = true;
                    emergencyStop = false;
                    actionThread = std::thread(performLongRunningTask);
                } else {
                    std::cout << "Task already running.\n";
                }
            } break;
        }
    } else {
        std::cerr << "Failed to parse protobuf message.\n";
    }
}

void performLongRunningTask() {
    std::unique_lock<std::mutex> lock(taskMutex);

    while (taskRunning && !emergencyStop) {
        // Release lock and wait for either stop signal or timeout
        auto status =
            taskCondition.wait_for(lock, std::chrono::seconds(1),
                                   [&] { return emergencyStop.load(); });
        
        if (!emergencyStop) {
            std::cout << "Task running...\n";
        }
    }

    if (emergencyStop) {
        std::cout << "Task interrupted by emergency stop.\n";
    } else {
        std::cout << "Task completed successfully.\n";
    }

    taskRunning = false;
    emergencyStop = false;
}

void stopTask() {
    std::cout << "Stopping the running task...\n";
    {
        std::lock_guard<std::mutex> lock(taskMutex);
        if (!taskRunning) {
            std::cout << "No task is currently running.\n";
            return;
        }
        emergencyStop = true;
    }
    taskCondition.notify_all();

    if (actionThread.joinable()) {
        actionThread.join();
    }
}
