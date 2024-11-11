#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include "constants.h"

int main()
{
    const char *message = "Hello from the sender!";

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in receiver_addr;
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(UDP_PORT);
    if (inet_pton(AF_INET, ETHERNET_IP, &receiver_addr.sin_addr) <= 0)
    {
        perror("Invalid address or address not supported");
        close(sockfd);
        return 1;
    }

    if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0)
    {
        perror("Failed to send message");
        close(sockfd);
        return 1;
    }

    close(sockfd);
    return 0;
}