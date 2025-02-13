#include "uart.h"

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <cstring>
#include <errno.h>
#include <iostream>

UARTException::UARTException(const std::string &msg)
    : std::runtime_error(msg)
{
}

UART::UART(const std::string &port)
{
    fd_ = openPort(port);
}

UART::~UART()
{
    closePort();
}

ssize_t UART::readData(char *buffer, size_t bufferSize)
{
    ssize_t bytesRead = ::read(fd_, buffer, bufferSize);
    if (bytesRead < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0; // No data available
        throw UARTException("UART read error: " + std::string(strerror(errno)));
    }
    return bytesRead;
}

ssize_t UART::writeData(const char *data, size_t size)
{
    ssize_t bytesWritten = ::write(fd_, data, size);
    if (bytesWritten < 0)
    {
        throw UARTException("UART write error: " + std::string(strerror(errno)));
    }
    return bytesWritten;
}

int UART::getFd() const noexcept
{
    return fd_;
}

bool UART::isOpen() const
{
    return (fd_ >= 0);
}

void UART::closePort()
{
    if (fd_ >= 0)
    {
        ::close(fd_);
        fd_ = -1;
    }
}

int UART::openPort(const std::string &port)
{
    int fd = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
    {
        throw UARTException("Failed to open UART port: " + port);
    }
    configurePort(fd, B9600);
    return fd;
}

void UART::configurePort(int fd, int baudRate)
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