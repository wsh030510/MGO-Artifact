#!/bin/bash

# Define directories containing C source files
DIRECTORIES=("test" "test1" "test2")

echo "Starting batch conversion of C files to LLVM IR (.ll)..."

for dir in "${DIRECTORIES[@]}"; do
    if [ ! -d "$dir" ]; then
        echo "Warning: Directory '$dir' not found. Skipping."
        continue
    fi

    for c_file in "$dir"/*.c; do
        if [ ! -e "$c_file" ]; then
            continue
        fi

        filename=$(basename "$c_file" .c)
        ll_file="$dir/$filename.ll"

        echo -n "Converting $c_file ... "

        # Execute clang conversion command, ensuring -O0 is used to disable optimization
        clang -emit-llvm -S -O0 "$c_file" -o "$ll_file" 2>/dev/null

        if [ $? -eq 0 ]; then
            echo "SUCCESS"
        else
            echo "FAILED (Check syntax or dependencies)"
        fi
    done
done

echo "Conversion complete. Ready for Nidhugg testing."