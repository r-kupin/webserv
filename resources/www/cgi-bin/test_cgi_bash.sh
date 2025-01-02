#!/bin/bash
data=""
while IFS= read -r -N100 -d '' chunk; do
    data="$data$chunk"
done
data="$data$chunk"
data_size=$(echo -n "$data" | wc -c)

echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"
echo "Content-Length: $((data_size))"
echo ""

echo "$data" 2>err_out
echo "$data" 1>out 2>err_file