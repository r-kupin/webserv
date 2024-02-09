#!/usr/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <filename> <port>"
    exit 1
fi

filename="$1"
port="$2"

exec 3<> /dev/tcp/localhost/"$port"  # Open a connection to the server on port 4280

# Send data to the server
cat "$filename" >&3

# Close the connection
exec 3>&-
