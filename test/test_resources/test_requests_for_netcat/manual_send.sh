#!/usr/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

filename="$1"

exec 3<> /dev/tcp/localhost/4281  # Open a connection to the server on port 4280

# Send data to the server
cat "$filename" >&3

# Close the connection
exec 3>&-
