#!/bin/bash
if ! command -v socat &> /dev/null; then
    echo "socat is not installed. Please install via your package manager."
    exit 1
fi

# Define paths and ports
SERIAL_SENDER_PORT="/tmp/tty.sender"
SERIAL_RECEIVER_PORT="/tmp/tty.receiver"
UDP_SENDER_PORT="127.0.0.1:5000"
UDP_RECEIVER_PORT="127.0.0.1:5001"

# Kill any existing socat processes
pkill -f socat

# Set up trap for cleanup on exit
cleanup() {
    echo "Cleaning up..."
    kill $SERIAL_SOCAT_PID $UDP_SOCAT_PID 2>/dev/null
    rm -f $SERIAL_SENDER_PORT $SERIAL_RECEIVER_PORT
    exit 0
}
trap cleanup EXIT

# Create the virtual serial ports
echo "Starting socat to create virtual serial ports..."
socat -d -d PTY,raw,echo=0,link=$SERIAL_SENDER_PORT PTY,raw,echo=0,link=$SERIAL_RECEIVER_PORT &
SERIAL_SOCAT_PID=$!
sleep 1

# Verify serial ports were created successfully
if [ ! -e "$SERIAL_SENDER_PORT" ] || [ ! -e "$SERIAL_RECEIVER_PORT" ]; then
    echo "Error: Failed to create virtual serial ports"
    exit 1  # This triggers cleanup
fi

echo "Virtual serial ports created:"
echo "  Sender: $SERIAL_SENDER_PORT"
echo "  Receiver: $SERIAL_RECEIVER_PORT"

# Create the virtual UDP connection
echo "Starting socat to create virtual UDP connection..."
socat -d -d UDP-LISTEN:5000,reuseaddr,fork UDP-LISTEN:5001,reuseaddr,fork &
UDP_SOCAT_PID=$!
sleep 1

# Verify UDP connection was created successfully
if ! ps -p $UDP_SOCAT_PID > /dev/null; then
    echo "Error: Failed to create UDP connection"
    exit 1  # This triggers cleanup
fi

echo "Virtual UDP connection established:"
echo "  Sender: $UDP_SENDER_PORT"
echo "  Receiver: $UDP_RECEIVER_PORT"

echo "All connections established successfully."
echo "Press Ctrl+C to terminate all connections..."

# Keep the script running to allow Socat processes to continue
while true; do
    sleep 1
done
