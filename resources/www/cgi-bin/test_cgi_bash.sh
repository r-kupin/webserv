#!/bin/bash
echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"
sleep 3;
data=""
while IFS= read -r line; do
    data="$data$line\n"
done
data_size=$(echo -e "$data" | wc -c)
#data_size=0

echo "Content-Length: $((data_size))"
echo ""

echo -e "$data"