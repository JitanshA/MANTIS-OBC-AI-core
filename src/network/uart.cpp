#include "uart.h"
#include "utils.h"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

UARTPort::UARTPort(const std::string &device, int baudRate)
    : device_(device), baudRate_(baudRate), isConfigured_(false)
{

    port_ = open(device.c_str(), O_RDWR);
    if (port_ == -1)
    {
        logMessage("Error opening port: " + std::string(strerror(errno)));
    }
    else
    {
        logMessage("Opened UART port on " + device_);
    }
}

UARTPort::~UARTPort()
{
    if (port_ != -1)
    {
        close(port_);
        logMessage("Closed UART port on " + device_);
    }
}

bool UARTPort::configurePort()
{
    struct termios tty;
    if (tcgetattr(port_, &tty) != 0)
    {
        logMessage("Error from tcgetattr: " + std::string(strerror(errno)));
        return false;
    }

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    cfsetispeed(&tty, baudRate_);
    cfsetospeed(&tty, baudRate_);

    if (tcsetattr(port_, TCSANOW, &tty) != 0)
    {
        logMessage("Error from tcsetattr: " + std::string(strerror(errno)));
        return false;
    }

    isConfigured_ = true;
    logMessage("UART port configured on " + device_ + " with baud rate " + std::to_string(baudRate_));
    return true;
}

bool UARTPort::readCommand(command::Command &cmd)
{
    if (!isConfigured_)
    {
        logMessage("Attempted to read command, but UART port is not configured.");
        return false;
    }

    std::vector<char> buffer(BUFFER_SIZE);
    int num_bytes = read(port_, buffer.data(), buffer.size());
    if (num_bytes < 0)
    {
        logMessage("Error reading from UART port: " + std::string(strerror(errno)));
        return false;
    }

    if (!cmd.ParseFromArray(buffer.data(), num_bytes))
    {
        logMessage("Failed to parse command from received data.");
        return false;
    }

    logMessage("Command received and parsed successfully.");
    return true;
}