#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1" // Receiver IP address
#define SERVER_PORT 5001      // Receiver port

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <image file path>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file)
    {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in receiver_addr;
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &receiver_addr.sin_addr) <= 0)
    {
        perror("Invalid address or address not supported");
        close(sockfd);
        return 1;
    }

    size_t chunk_size = 1024;
    for (size_t i = 0; i < buffer.size(); i += chunk_size)
    {
        size_t bytes_to_send = std::min(chunk_size, buffer.size() - i);
        if (sendto(sockfd, buffer.data() + i, bytes_to_send, 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0)
        {
            perror("Failed to send data");
            close(sockfd);
            return 1;
        }
    }

    std::cout << "Image file sent successfully!" << std::endl;
    close(sockfd);
    return 0;
}