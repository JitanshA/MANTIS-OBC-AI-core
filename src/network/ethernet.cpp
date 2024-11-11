#include "ethernet.h"
#include "utils.h"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include "constants.h"

EthernetPort::EthernetPort(int port)
{
    udpSocket_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket_ < 0)
    {
        logMessage("Error: Socket creation failed.");
        perror("Socket creation failed");
    }

    memset(&receiverAddr_, 0, sizeof(receiverAddr_));
    receiverAddr_.sin_family = AF_INET;
    receiverAddr_.sin_addr.s_addr = INADDR_ANY;
    receiverAddr_.sin_port = htons(port);

    if (bind(udpSocket_, (struct sockaddr *)&receiverAddr_, sizeof(receiverAddr_)) < 0)
    {
        logMessage("Error: Bind failed.");
        perror("Bind failed");
        close(udpSocket_);
    }
}

EthernetPort::~EthernetPort()
{
    close(udpSocket_);
    logMessage("UDP socket closed.");
}

bool EthernetPort::receiveImage(const std::string &outputFile)
{
    std::ofstream file(outputFile, std::ios::binary);
    if (!file)
    {
        logMessage("Error: Failed to open file for writing - " + outputFile);
        return false;
    }

    char buffer[BUFFER_SIZE];
    struct sockaddr_in senderAddr;
    socklen_t senderLen = sizeof(senderAddr);
    ssize_t bytesReceived;

    logMessage("Waiting for image data on port " + std::to_string(ntohs(receiverAddr_.sin_port)) + "...");

    while ((bytesReceived = recvfrom(udpSocket_, buffer, sizeof(buffer), 0, (struct sockaddr *)&senderAddr, &senderLen)) > 0)
    {
        file.write(buffer, bytesReceived);
    }

    if (bytesReceived < 0)
    {
        logMessage("Error: Receiving data failed.");
        perror("Receiving data failed");
        return false;
    }

    logMessage("Image received and saved to " + outputFile);
    return true;
}

bool EthernetPort::receiveMessage(std::string &message)
{
    char buffer[BUFFER_SIZE];
    struct sockaddr_in senderAddr;
    socklen_t senderLen = sizeof(senderAddr);

    ssize_t bytesReceived = recvfrom(udpSocket_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&senderAddr, &senderLen);
    if (bytesReceived < 0)
    {
        logMessage("Error: Receiving text message failed.");
        perror("Receiving text message failed");
        return false;
    }

    // Null-terminate and store in message string
    buffer[bytesReceived] = '\0';
    message = buffer;
    logMessage("Text message received: " + message);
    return true;
}
