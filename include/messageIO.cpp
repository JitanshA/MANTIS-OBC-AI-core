#include "messageIO.h"
#include "uart.h"
#include "protocol.pb.h"

#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>

std::optional<std::string> readLengthPrefixedMessage(UART &uart, int maxMessageSize)
{
    // Read 4-byte length prefix
    uint32_t netLen = 0;
    char *netLenPtr = reinterpret_cast<char *>(&netLen);
    int toRead = 4;
    int readSoFar = 0;

    while (readSoFar < toRead)
    {
        ssize_t n = uart.readData(netLenPtr + readSoFar, toRead - readSoFar);
        if (n < 0)
        {
            // If it's a transient "no data" condition
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            std::cerr << "[ERROR] readLengthPrefixedMessage: error reading length prefix: "
                      << strerror(errno) << "\n";
            return std::nullopt;
        }
        else if (n == 0)
        {
            // No data yet, continue until we get all 4 bytes
            continue;
        }
        readSoFar += static_cast<int>(n);
    }

    // Convert length from network to host byte order
    uint32_t msgLen = ntohl(netLen);
    if (static_cast<int>(msgLen) > maxMessageSize)
    {
        std::cerr << "[ERROR] Incoming message too large: " << msgLen << " bytes\n";
        return std::nullopt;
    }

    // Read the actual message bytes
    std::string msgData;
    msgData.resize(msgLen);
    char *bufPtr = &msgData[0];
    readSoFar = 0;

    while (readSoFar < static_cast<int>(msgLen))
    {
        ssize_t n = uart.readData(bufPtr + readSoFar, msgLen - readSoFar);
        if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            std::cerr << "[ERROR] readLengthPrefixedMessage: error reading message data: "
                      << strerror(errno) << "\n";
            return std::nullopt;
        }
        else if (n == 0)
        {
            continue;
        }
        readSoFar += static_cast<int>(n);
    }

    return msgData;
}

bool parseAndPrintCommand(const std::string &serialized)
{
    command::Command incoming;
    if (!incoming.ParseFromString(serialized))
    {
        std::cerr << "[ERROR] Failed to parse incoming command.\n";
        return false;
    }

    // Print the Command fields
    std::cout << "\n[SERVER] Received Command:\n";
    std::cout << "   cmd = " << incoming.cmd() << "\n";
    std::cout << "   src = " << incoming.src() << "\n";
    std::cout << "   dst = " << incoming.dst() << "\n";
    std::cout << "   dtt = " << incoming.dtt() << "\n";
    std::cout << "   sig = " << incoming.sig() << "\n";

    if (incoming.has_string_d())
    {
        std::cout << "   data.string_d = " << incoming.string_d() << "\n";
    }
    else if (incoming.has_bytes_d())
    {
        std::cout << "   data.bytes_d (size) = " << incoming.bytes_d().size() << "\n";
    }
    else if (incoming.has_int_d())
    {
        std::cout << "   data.int_d = " << incoming.int_d() << "\n";
    }
    else if (incoming.has_double_d())
    {
        std::cout << "   data.double_d = " << incoming.double_d() << "\n";
    }
    else
    {
        std::cout << "   data not set\n";
    }

    return true;
}