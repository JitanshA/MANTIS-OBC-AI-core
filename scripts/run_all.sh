#!/bin/bash

# Define the paths for the build directory and the scripts directory
BUILD_DIR="../build"
SCRIPTS_DIR="$(dirname "$0")"

# Run create_links.sh to set up virtual ports
if [[ -x "$SCRIPTS_DIR/create_links.sh" ]]; then
    echo "Running create_links.sh to set up virtual ports..."
    "$SCRIPTS_DIR/create_links.sh"
else
    echo "Error: create_links.sh not found or not executable."
    exit 1
fi

# Function to terminate all background processes on exit, including socat
cleanup() {
    echo "Cleaning up..."
    pkill -f socat  # Terminate all socat processes
    kill $OBC_AI_PID $OBC_MICRO_PID $PAYLOAD_PID 2>/dev/null
    echo "All processes terminated."
}
trap cleanup EXIT

# Step 1: Start obc_ai in the background to listen for commands
echo "Starting obc_ai..."
"$BUILD_DIR/obc_ai" &
OBC_AI_PID=$!
sleep 2  # Give it time to start up

# Step 2: Send the UART command by running obc_micro
echo "Sending UART command with obc_micro..."
"$BUILD_DIR/obc_micro" &
OBC_MICRO_PID=$!
sleep 2  # Wait for the command to be processed

# Step 3: Start payload to send a message over the Ethernet port
echo "Starting payload to send the text message..."
"$BUILD_DIR/payload" &
PAYLOAD_PID=$!

# Wait for payload to finish sending
wait $PAYLOAD_PID

# Allow some time for logs and final processing before the script exits
sleep 2

echo "Process completed. Check logs for details."