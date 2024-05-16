#!/bin/bash
data=""
while IFS= read -r line; do
    data="$data$line\n"
done

echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"

echo ""
echo "<html><head>"
echo "  <meta charset="utf-8" />"
echo "  <title>CGI</title>"
echo "</head><body>"
#echo $REQUEST_METHOD
#echo $QUERY_STRING
#echo $SERVER_PROTOCOL
echo -e "$data"
echo "</body></html>"
