#!/bin/bash

CLIENT_PORT="/tmp/tty.client"
SERVER_PORT="/tmp/tty.server"

cleanup() {
    echo -e "\nStopping virtual connection..."
    kill $SOCAT_PID 2>/dev/null
    wait $SOCAT_PID 2>/dev/null
    rm -f "$CLIENT_PORT" "$SERVER_PORT"
    echo "Cleanup complete. Exiting."
    exit 0
}

trap cleanup SIGINT

if [ -e "$CLIENT_PORT" ]; then
    rm -f "$CLIENT_PORT"
fi

if [ -e "$SERVER_PORT" ]; then
    rm -f "$SERVER_PORT"
fi

socat -d -d pty,raw,echo=0,link=$CLIENT_PORT pty,raw,echo=0,link=$SERVER_PORT &
SOCAT_PID=$!

sleep 1

echo "Virtual ports created:"
echo "Client Port: $CLIENT_PORT"
echo "Server Port: $SERVER_PORT"
echo "Press Ctrl+C to stop the virtual connection."

# Keep script running to catch SIGINT
while true; do
    sleep 1
done
