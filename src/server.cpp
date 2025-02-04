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
#include <termios.h>

#include "protocol.pb.h"

#define SERVER_PORT "/tmp/tty.server"
#define BUFFER_SIZE 256

class UART
{
public:
    explicit UART(const std::string &port)
    {
        fd_ = openPort(port);
    }
    ~UART()
    {
        if (fd_ >= 0)
            close(fd_);
    }

    ssize_t readData(char *buffer, ssize_t bufferSize)
    {
        ssize_t bytesRead = read(fd_, buffer, bufferSize);
        return bytesRead;
    }

    ssize_t writeData(const char *data, size_t size)
    {
        ssize_t bytesWritten = write(fd_, data, size);
        return bytesWritten;
    }

private:
    int fd_;

    int openPort(const std::string &port)
    {
        int fd = open(port.c_str(), O_RDWR | O_NOCTTY); // is this blocking?
        configurePort(B9600);
        return fd;
    }

    void configurePort(int baudRate)
    {
        struct termios tty;

        // Control modes
        tty.c_cflag |=
            (CLOCAL | CREAD);    // Enable receiver, ignore modem control lines
        tty.c_cflag &= ~CSIZE;   // Clear character size bits
        tty.c_cflag |= CS8;      // Set character size to 8 bits
        tty.c_cflag &= ~PARENB;  // Disable parity bit
        tty.c_cflag &= ~CSTOPB;  // Use one stop bit
        tty.c_cflag &= ~CRTSCTS; // Disable hardware flow control

        // Input modes
        tty.c_iflag &=
            ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);

        // Output modes
        tty.c_oflag &= ~OPOST;

        // Local modes
        tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

        tty.c_cc[VMIN] = 1;  // Minimum number of characters for non-canonical read
        tty.c_cc[VTIME] = 1; // Timeout in deciseconds for non-canonical read DOUBLE CHECK THIS

        cfsetispeed(&tty, baudRate);
        cfsetospeed(&tty, baudRate);
    }
};

class TaskManager
{
public:
    TaskManager() : taskRunning(false), emergencyStop(false) {}

    void start()
    {
        std::lock_guard<std::mutex> lock(taskMutex);
        if (taskRunning)
        {
            std::cout << "Task already running.\n";
            return;
        }

        taskRunning = true;
        emergencyStop = false;
        actionThread = std::thread(&TaskManager::runTask, this);
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(taskMutex);
            if (!taskRunning)
            {
                std::cout << "No task running to stop.\n";
                return;
            }
            emergencyStop = true;
        }
        taskCondition.notify_all();
        if (actionThread.joinable())
        {
            actionThread.join();
        }
    }

    ~TaskManager()
    {
        stop();
    }

private:
    void runTask()
    {
        std::unique_lock<std::mutex> lock(taskMutex);
        while (taskRunning && !emergencyStop)
        {
            taskCondition.wait_for(lock, std::chrono::seconds(1), [this]
                                   { return this->emergencyStop.load(); });
            if (!emergencyStop)
            {
                std::cout << "Task running...\n";
            }
        }
        taskRunning = false;
    }

    std::atomic<bool> taskRunning;
    std::atomic<bool> emergencyStop;
    std::thread actionThread;
    std::mutex taskMutex;
    std::condition_variable taskCondition;
};

class CommandHandler
{
public:
    explicit CommandHandler(TaskManager &taskManagerRef) : taskManager(taskManagerRef) {}

    void parseAndExecute(const std::string &receivedData)
    {
        command::Command msg;
        if (!msg.ParseFromString(receivedData))
        {
            throw std::runtime_error("Failed to parse Protobuf message.");
        }

        switch (msg.cmd())
        {
        case 99:
            taskManager.stop();
            break;
        case 1:
            taskManager.start();
            break;
        default:
            std::cout << "Unknown command: " << msg.cmd() << "\n";
        }
    }

private:
    TaskManager &taskManager;
};

class JetsonCore
{
public:
    JetsonCore(const std::string &port) : uart(port), taskManager(), commandHandler(taskManager) {}

    void run()
    {
        char buffer[BUFFER_SIZE];
        while (!emergencyStop)
        {
            try
            {
                ssize_t bytesRead = uart.readData(buffer, sizeof(buffer));
                std::string data(buffer, bytesRead);
                commandHandler.parseAndExecute(data);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error: " << e.what() << "\n";
            }
        }
    }

private:
    UART uart;
    CommandHandler commandHandler;
    TaskManager taskManager;
    std::atomic<bool> emergencyStop{false};
};

int main()
{
    JetsonCore app(SERVER_PORT);
    app.run();

    return EXIT_SUCCESS;
}
