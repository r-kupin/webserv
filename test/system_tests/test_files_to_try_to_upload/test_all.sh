#!/bin/bash

# Check if a directory is provided as a command-line argument
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <directory> <url>"
    exit 1
fi

target_directory="$1"
url="$2"
rm "$1"/*

((i=0));
# Iterate over files with ".txt" or ".iso" extensions in the current directory
for file in *.txt *.iso; do
    if [ -f "$file" ]; then
        printf "%s\t" "$file"
        # Form the path to the corresponding file in the target directory
        target_file="${target_directory}/$i"

        curl -F "file=@$file" "$url"
        # Check if the file exists in the target directory
        if [ -f "$target_file" ]; then
            printf "\t%s\n" "$i"
            # Perform 'diff' between the current file and the corresponding file in the target directory
            diff_result=$(diff "$file" "$target_file")
            echo -n "diff: "
            if [ -z "$diff_result" ]; then
                printf "OK"
            else
                file_size=$(stat -c%s "$file")
                printf "\n\t%s size: %d\n" "$file" "$file_size"

                file_size=$(stat -c%s "$target_file")
                printf "\t%s size: %d\n" "$i" "$file_size"

#                printf "%s\n" "$diff_result"
            fi
            ((i++));
            echo "";
        else
            echo "File $i not found in the target directory.
            Probably, upload failed."
        fi
    fi
done