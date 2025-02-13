#ifndef MESSAGE_IO_H
#define MESSAGE_IO_H

#include <string>
#include <optional>

class UART;

// Reads a length-prefixed message (4-byte network-order length + data).
// Returns the raw serialized data on success, or std::nullopt on error.
//
// This function blocks until it has read the full 4-byte length and
// the indicated number of data bytes (unless an error occurs).
std::optional<std::string> readLengthPrefixedMessage(UART &uart, int maxMessageSize = 4096);

// Parses a serialized command::Command and prints out all fields.
// Returns true on success, false on parse failure.
bool parseAndPrintCommand(const std::string &serialized);

#endif // MESSAGE_IO_H