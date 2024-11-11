#include "protocol.pb.h"
#include <fcntl.h>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include "constants.h"
#include "utils.h"

class Sender
{
public:
    Sender(const std::string &device)
    {
        port_ = open(device.c_str(), O_RDWR);
        if (port_ == -1)
        {
            std::cerr << "Error opening port: " << strerror(errno) << std::endl;
            return;
        }
        configurePort();
    }

    ~Sender()
    {
        if (port_ != -1)
        {
            close(port_);
        }
    }

    bool configurePort()
    {
        struct termios tty;
        if (tcgetattr(port_, &tty) != 0)
        {
            std::cerr << "Error from tcgetattr: " << strerror(errno) << std::endl;
            return false;
        }

        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
        tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
        tty.c_oflag &= ~OPOST;

        tty.c_cc[VMIN] = 1;
        tty.c_cc[VTIME] = 1;

        cfsetispeed(&tty, BAUD_RATE);
        cfsetospeed(&tty, BAUD_RATE);

        if (tcsetattr(port_, TCSANOW, &tty) != 0)
        {
            logMessage("Error from tcsetattr: " + std::string(strerror(errno)));
            return false;
        }

        return true;
    }

    bool sendCommand(const command::Command &cmd)
    {
        std::string serialized;
        if (!cmd.SerializeToString(&serialized))
        {
            std::cerr << "Failed to serialize command" << std::endl;
            return false;
        }

        ssize_t bytes_written = write(port_, serialized.c_str(), serialized.length());
        if (bytes_written < 0)
        {
            std::cerr << "Error writing to port: " << strerror(errno) << std::endl;
            return false;
        }

        return true;
    }

private:
    int port_;
};

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Sender sender("/tmp/tty.sender");

    command::Command cmd;
    cmd.set_cmd(3);
    cmd.set_src(command::IPC);
    cmd.set_dst(command::OBA);
    cmd.set_dtt(100);
    cmd.set_sig(1);
    cmd.set_int_d(123456);

    // Send the command every second
    if (sender.sendCommand(cmd))
    {
        std::cout << "Sent message successfully\n";
    }
    else
    {
        std::cerr << "Failed to send message\n";
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}