#include <fcntl.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <sys/select.h>
#include <thread>
#include <termios.h>

#include "protocol.pb.h"

#define SERVER_PORT "/tmp/tty.server"
#define BUFFER_SIZE 256
#define MAX_RETRIES 5
#define RETRY_DELAY_MS 2000 // 2 seconds
#define READ_TIMEOUT_SEC 1  // 1 second timeout for select()

/**
 * @class UARTException
 * @brief
 */
class UARTException : public std::runtime_error
{
public:
    explicit UARTException(const std::string &msg) : std::runtime_error(msg) {}
};

/**
 * @class UART
 * @brief Encapsulates UART configuration, opening, reading, and writing.
 */
class UART
{
public:
    explicit UART(const std::string &port)
    {
        fd_ = openPort(port);
    }

    ~UART()
    {
        closePort();
    }

    ssize_t readData(char *buffer, size_t bufferSize)
    {
        ssize_t bytesRead = read(fd_, buffer, bufferSize);
        if (bytesRead < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return 0; // No data available
            throw UARTException("UART read error: " + std::string(strerror(errno)));
        }
        return bytesRead;
    }

    ssize_t writeData(const char *data, size_t size)
    {
        ssize_t bytesWritten = write(fd_, data, size);
        if (bytesWritten < 0)
        {
            throw UARTException("UART write error: " + std::string(strerror(errno)));
        }
        return bytesWritten;
    }

    int getFd() const noexcept
    {
        return fd_;
    }

    bool isOpen() const { return fd_ >= 0; }

    void closePort()
    {
        if (fd_ >= 0)
        {
            close(fd_);
            fd_ = -1;
        }
    }

private:
    int fd_;

    int openPort(const std::string &port)
    {
        int fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd < 0)
        {
            throw UARTException("Failed to open UART port: " + port);
        }
        configurePort(fd, B9600);
        return fd;
    }

    void configurePort(int fd, int baudRate)
    {
        struct termios tty;
        if (tcgetattr(fd, &tty) != 0)
        {
            throw UARTException("Failed to get UART attributes.");
        }

        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
        tty.c_oflag &= ~OPOST;
        tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

        tty.c_cc[VMIN] = 1;
        tty.c_cc[VTIME] = 1;

        cfsetispeed(&tty, baudRate);
        cfsetospeed(&tty, baudRate);

        if (tcsetattr(fd, TCSANOW, &tty) != 0)
        {
            throw UARTException("Failed to set UART attributes.");
        }
    }
};

void uartReadLoop(UART &uart, std::atomic<bool> &running)
{
    char buffer[BUFFER_SIZE];

    while (running)
    {
        try
        {
            fd_set read_fds;
            struct timeval timeout;

            FD_ZERO(&read_fds);
            FD_SET(uart.getFd(), &read_fds); // Monitor UART file descriptor

            timeout.tv_sec = READ_TIMEOUT_SEC;
            timeout.tv_usec = 0;

            int activity = select(uart.getFd() + 1, &read_fds, nullptr, nullptr, &timeout);

            if (activity < 0)
            {
                if (errno == EINTR)
                    continue; // Interrupted by a signal, restart loop
                throw UARTException("select() failed: " + std::string(strerror(errno)));
            }
            else if (activity == 0)
            {
                // Timeout reached, continue loop (non-blocking)
                continue;
            }

            if (FD_ISSET(uart.getFd(), &read_fds))
            {
                ssize_t bytesRead = uart.readData(buffer, sizeof(buffer));
                if (bytesRead > 0)
                {
                    std::string data(buffer, bytesRead);
                    std::cout << "[UART] Received: " << data << std::endl;
                }
            }
        }
        catch (const UARTException &e)
        {
            std::cerr << "[ERROR] UART read error: " << e.what() << std::endl;
            running = false; // Stop loop on critical error
        }
        catch (const std::exception &e)
        {
            std::cerr << "[FATAL] Unexpected error: " << e.what() << std::endl;
            running = false;
        }
    }

    std::cout << "[UART] Read loop exiting..." << std::endl;
}

int main()
{

    std::unique_ptr<UART> uart; // Declare a smart pointer that is big enough for UART class
    int attempts = 0;
    std::atomic<bool> running(true);

    while (attempts < MAX_RETRIES)
    {
        try
        {
            uart = std::make_unique<UART>(SERVER_PORT); // Allocate memory to object "uart" and call constructor, replace previous instance
            std::cout << "[SUCCESS] UART initialized on port " << SERVER_PORT << "\n";
            break; // Exit loop on success
        }
        catch (const UARTException &e)
        {
            std::cerr << "[ERROR] UART initialization failed: " << e.what() << "\n";
            attempts++;
            if (attempts >= MAX_RETRIES)
            {
                std::cerr << "[FATAL] Unable to initialize UART after " << MAX_RETRIES << " attempts.\n";
                return EXIT_FAILURE;
            }

            std::cerr << "[RETRY] Retrying in " << (RETRY_DELAY_MS / 1000) << " seconds...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS)); // Delay before retrying
        }
        catch (const std::exception &e)
        {
            std::cerr << "[FATAL] Unexpected error during UART initialization: " << e.what() << "\n";
            return EXIT_FAILURE;
        }
    }

    uartReadLoop(*uart, running);

    return EXIT_SUCCESS;
}
