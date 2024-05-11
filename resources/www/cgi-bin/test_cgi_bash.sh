#!/bin/bash
echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"

echo ""
echo "<html><head>"
echo "  <meta charset="utf-8" />"
echo "  <title>CGI</title>"
echo "</head><body>"
for (( j = 0; j < 10; j++ )); do
    for (( i = 0; i < 10; i++ )); do
        echo "Test CGI Script"
    done
    sleep 1
done
echo "</body></html>"
