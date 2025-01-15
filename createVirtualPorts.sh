#!/bin/bash

CLIENT_PORT="/tmp/tty.client"
SERVER_PORT="/tmp/tty.server"

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

echo "To stop the virtual connection, kill socat with: kill $SOCAT_PID"

exit 0
