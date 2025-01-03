#!/bin/bash
data=""
while IFS= read -r -N100 -d '' chunk; do
    data="$data$chunk"
done
data="$data$chunk"
data_size=$(echo "$data" | wc -c)

echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"
echo "Content-Length: $((data_size))"
echo ""

echo "$data"