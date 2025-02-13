#include "protocol.pb.h"
#include "uart.h"
#include <arpa/inet.h> // htonl()
#include <cstdlib>     // EXIT_FAILURE
#include <string>

#define CLIENT_PORT "/tmp/tty.client"

int main()
{

    // Open client
    UART clientUart(CLIENT_PORT);
    if (!clientUart.isOpen())
        return EXIT_FAILURE;

    command::Command cmdMsg;
    cmdMsg.set_cmd(123);
    cmdMsg.set_src(command::IPC);
    cmdMsg.set_dst(command::ADC);
    cmdMsg.set_dtt(9999);
    cmdMsg.set_sig(42);
    cmdMsg.set_int_d(123456789);

    // Serialize
    std::string outBuf;
    if (!cmdMsg.SerializeToString(&outBuf))
        return EXIT_FAILURE;

    // Write the length prefix (4 bytes) + message
    uint32_t netLen = htonl(static_cast<uint32_t>(outBuf.size()));

    // Write the prefix
    clientUart.writeData(reinterpret_cast<const char *>(&netLen), sizeof(netLen));
    // Write the serialized data
    clientUart.writeData(outBuf.data(), outBuf.size());

    // 5. Done.
    return 0;
}