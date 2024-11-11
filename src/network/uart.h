#ifndef UART_H
#define UART_H

#include "protocol.pb.h"
#include <string>
#include <vector>
#include "constants.h"

class UARTPort
{
public:
    UARTPort(const std::string &device, int baudRate = BAUD_RATE);
    ~UARTPort();
    bool configurePort();
    bool readCommand(command::Command &cmd);

private:
    int port_;
    std::string device_;
    int baudRate_;
    bool isConfigured_;
};

#endif // UART_H