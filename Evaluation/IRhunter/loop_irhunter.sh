#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <test_file_path>"
    echo "Example: $0 Layer1/21-crossbeam.c"
    exit 1
fi

TARGET_FILE="$1"
MAX_ATTEMPTS=10
SUCCESS_KEYWORD="YOUR_SUCCESS_KEYWORD" 

echo "Starting continuous test for: $TARGET_FILE"
echo "Max attempts: $MAX_ATTEMPTS"
echo "------------------------------------------------"

for (( i=1; i<=MAX_ATTEMPTS; i++ ))
do
    echo "Attempt $i: Running dynamic data capture..."
    
    ./dynamic_run.sh "$TARGET_FILE" > /dev/null 2>&1
    
    if [ $? -ne 0 ]; then
        echo "Warning: dynamic_run.sh failed, skipping analysis."
        continue
    fi

    echo "Attempt $i: Running Maven analysis..."
    
    MVN_OUT=$(mvn exec:java -Dexec.mainClass="tju.edu.cn.reorder.ReorderMain" 2>&1 | tee /dev/tty)
    
    if echo "$MVN_OUT" | grep -q "$SUCCESS_KEYWORD"; then
        echo "------------------------------------------------"
        echo "Success: Target hit on attempt $i."
        echo "------------------------------------------------"
        exit 0
    fi
    
    echo "Missed. Retrying..."
    sleep 1
done

echo "------------------------------------------------"
echo "Test finished. Reached max attempts ($MAX_ATTEMPTS) without capturing target output."