#ifndef UART_H
#define UART_H

#include <stdexcept>
#include <string>

class UARTException : public std::runtime_error
{
public:
    explicit UARTException(const std::string &msg);
};

class UART
{
public:
    explicit UART(const std::string &port);
    ~UART();

    ssize_t readData(char *buffer, size_t bufferSize);
    ssize_t writeData(const char *data, size_t size);

    int getFd() const noexcept;
    bool isOpen() const;
    void closePort();

private:
    int fd_;

    int openPort(const std::string &port);
    void configurePort(int fd, int baudRate);
};

#endif // UART_H