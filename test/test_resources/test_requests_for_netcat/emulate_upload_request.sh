#!/usr/bin/bash

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <hostname> <port> <file>"
    exit 1
fi

hostname=$1
port=$2
file=$3
request_file="${file%.*}_request.txt"

boundary="------------------------bc7b290799595db6"
pre_body=$(printf "%s\r\nContent-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\nContent-Type: text/plain\r\n\r\n" "--$boundary" "$file")
after_body=$(printf "\r\n--%s--\r\n" "$boundary")

body_size=$(($(printf "%s" "$pre_body" | wc -c) + \
          $(printf "%s" "$after_body" | wc -c) + \
          $(wc -c < "$file") + 2)) # printf skips tailing '\n' for no reason
printf "POST /uploads HTTP/1.1\r\n" > "$request_file"
printf "Host: %s:%s\r\n" "$hostname" "$port" >> "$request_file"
printf "User-Agent: curl/7.81.0\r\n" >> "$request_file"
printf "Accept: */*\r\n" >> "$request_file"
printf "Content-Length: %d\r\n" "$body_size" >> "$request_file"
printf "Content-Type: multipart/form-data; boundary=%s\r\n\r\n" "$boundary" >> "$request_file"
printf "%s\n" "$pre_body" >> "$request_file" # printf skips tailing '\n' for no reason

exec 3<> /dev/tcp/"$hostname"/"$port"

cat "$request_file"  >&3
cat "$file"  >&3
printf "%s\n" "$after_body" >&3

exec 3>&-