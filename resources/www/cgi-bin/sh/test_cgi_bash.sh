#!/bin/bash

# Print HTTP content type header
echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"
echo ""

# Start HTML output
echo "<html>"
echo "<head><title>Request Information</title></head>"
echo "<body>"
echo "<h1>Request Information</h1>"
echo "<h2>Headers</h2>"
echo "<ul>"

# Print all environment variables (headers in CGI are passed as environment variables)
for var in $(env)
do
    echo "<li>$var</li>"
done

echo "</ul>"
echo "<h2>Body</h2>"
echo "<pre>"

# Read the request body from standard input
if [ "$REQUEST_METHOD" = "POST" ]; then
    while read data
    do
        echo "$data"
    done
fi

echo "</pre>"
echo "</body></html>"
