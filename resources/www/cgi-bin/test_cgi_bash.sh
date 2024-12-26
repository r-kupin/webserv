#!/bin/bash
data=""
while IFS= read -r -n1 -d '' char; do
    data="$data$char"
done
data_size=$(echo -n "$data" | wc -c)

echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"
echo "Content-Length: $((data_size))"
echo ""

echo "$data"