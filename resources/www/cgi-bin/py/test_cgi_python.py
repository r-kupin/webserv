#!/usr/bin/env python3
import os
import sys

# Print HTTP content type header
print("Content-type: text/html")
print()

# Start HTML output
print("<html>")
print("<head><title>Request Information</title></head>")
print("<body>")
print("<h1>Request Information</h1>")
print("<h2>Headers</h2>")
print("<ul>")

# Environment variables contain headers and other CGI-related data
for key, value in os.environ.items():
    print(f"<li>{key}: {value}</li>")

print("</ul>")
print("<h2>Body</h2>")
print("<pre>")

# Read the request body from standard input if method is POST
if os.environ.get('REQUEST_METHOD', 'GET') == 'POST':
    print(sys.stdin.read())

print("</pre>")
print("</body></html>")
