#include "uart.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

void configureUart(int fd, int baudRate) {
    struct termios tty;

    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error: Unable to get serial port attributes\n";
        exit(EXIT_FAILURE);
    }

    // Control modes
    tty.c_cflag |=
        (CLOCAL | CREAD);     // Enable receiver, ignore modem control lines
    tty.c_cflag &= ~CSIZE;    // Clear character size bits
    tty.c_cflag |= CS8;       // Set character size to 8 bits
    tty.c_cflag &= ~PARENB;   // Disable parity bit
    tty.c_cflag &= ~CSTOPB;   // Use one stop bit
    tty.c_cflag &= ~CRTSCTS;  // Disable hardware flow control

    // Input modes
    tty.c_iflag &=
        ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);

    // Output modes
    tty.c_oflag &= ~OPOST;

    // Local modes
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    tty.c_cc[VMIN] = 1;   // Minimum number of characters for non-canonical read
    tty.c_cc[VTIME] = 1;  // Timeout in deciseconds for non-canonical read

    cfsetispeed(&tty, baudRate);
    cfsetospeed(&tty, baudRate);

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error: Unable to set serial port attributes\n";
        exit(EXIT_FAILURE);
    }
}

int openPort(const std::string& serverPort) {
    int fd = open(serverPort.c_str(), O_RDONLY | O_NOCTTY);
    if (fd == -1) {
        perror("Error opening port");
        return -1;
    }

    configureUart(fd, B9600);
    return fd;
}