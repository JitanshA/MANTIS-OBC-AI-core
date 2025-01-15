#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "protocol.pb.h"
#include "uart.hpp"

#define CLIENT_PORT "/tmp/tty.client"

void sendMessage(int fd, const command::Command& msg) {
    std::string serializedData;
    if (!msg.SerializeToString(&serializedData)) {
        std::cerr << "Error: Failed to serialize the message.\n";
        return;
    }

    ssize_t bytesWritten =
        write(fd, serializedData.data(), serializedData.size());
    if (bytesWritten < 0) {
        std::cerr << "Error: Unable to send data over the serial port.\n";
    } else {
        std::cout << "Sent " << bytesWritten << " bytes successfully.\n";
    }
}

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    int fd = open(CLIENT_PORT, O_WRONLY | O_NOCTTY);
    if (fd == -1) {
        std::cerr << "Error: Unable to open client port " << CLIENT_PORT
                  << "\n";
        return EXIT_FAILURE;
    }

    configureUart(fd, B9600);

    std::cout << "Client is ready to send data on " << CLIENT_PORT << "...\n";
    int cmdId = 0;

    while (true) {
        std::cout << "Enter cmd id: " << std::endl;
        std::cin >> cmdId;
        command::Command msg;
        msg.set_cmd(cmdId);                 // Example command value
        msg.set_src(command::Device::IPC);  // Source device
        msg.set_dst(command::Device::PAY);  // Destination device
        msg.set_dtt(12345);                 // Example timestamp
        msg.set_sig(67890);                 // Example signal value

        msg.set_string_d("Hello, Server!");

        sendMessage(fd, msg);
    }

    close(fd);

    return EXIT_SUCCESS;
}
