#!/bin/bash

# Check if socat is installed
if ! command -v socat &> /dev/null; then
    echo "socat is not installed. Please install it first."
    exit 1
fi

# Define virtual ports
UART_SENDER_PORT="/tmp/tty.sender"
UART_RECEIVER_PORT="/tmp/tty.receiver"
UDP_SENDER_PORT="127.0.0.1:5000"
UDP_RECEIVER_PORT="127.0.0.1:5001"

# Kill any existing socat processes to avoid conflicts
pkill -f socat

# Start socat for virtual UART connection
echo "Starting socat for virtual UART ports..."
socat -d -d PTY,raw,echo=0,link=$UART_SENDER_PORT PTY,raw,echo=0,link=$UART_RECEIVER_PORT &
sleep 1  # Give it a moment to initialize

# Start socat for virtual UDP connection
echo "Starting socat for virtual UDP ports..."
socat -d -d UDP-LISTEN:5000,reuseaddr,fork UDP-LISTEN:5001,reuseaddr,fork &
sleep 1  # Give it a moment to initialize

echo "Virtual ports created successfully."
echo "UART Sender: $UART_SENDER_PORT, UART Receiver: $UART_RECEIVER_PORT"
echo "UDP Sender: $UDP_SENDER_PORT, UDP Receiver: $UDP_RECEIVER_PORT"