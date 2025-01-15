#ifndef UART_HPP
#define UART_HPP

#include <termios.h>

#include <string>

void configureUart(int fd, int baudRate);
int openPort(const std::string& serverPort);

#endif  // UART_HPP
