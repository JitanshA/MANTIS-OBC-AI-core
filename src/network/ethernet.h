#ifndef ETHERNET_H
#define ETHERNET_H

#include <string>
#include <netinet/in.h>
#include "constants.h"

class EthernetPort
{
public:
    EthernetPort(int port = UDP_PORT);
    ~EthernetPort();
    bool receiveImage(const std::string &outputFile);
    bool receiveMessage(std::string &message);
    int getSocket() const { return udpSocket_; }

private:
    int udpSocket_;
    struct sockaddr_in receiverAddr_;
};

#endif // ETHERNET_H