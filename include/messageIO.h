#ifndef MESSAGE_IO_H
#define MESSAGE_IO_H

#include <string>
#include <optional>

class UART;

std::optional<std::string> readLengthPrefixedMessage(UART &uart, int maxMessageSize = 4096);

bool parseAndPrintCommand(const std::string &serialized);

#endif // MESSAGE_IO_H