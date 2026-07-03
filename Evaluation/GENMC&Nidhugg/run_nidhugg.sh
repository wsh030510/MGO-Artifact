#!/bin/bash

DIRECTORIES=("Layer1" "Layer2" "Layer3")
LOG_FILE="nidhugg_detailed.log"
SUMMARY_FILE="nidhugg_summary_report.csv"

# Initialize log file
echo "Nidhugg Detailed Log" > "$LOG_FILE"
echo "Start Time: $(date)" >> "$LOG_FILE"
echo "------------------------------------------------" >> "$LOG_FILE"

# Initialize CSV header
echo "Folder,File_Name,SC_Model_Result,PSO_Model_Result" > "$SUMMARY_FILE"

echo "Starting batch Nidhugg tests..."

for dir in "${DIRECTORIES[@]}"; do
    if [ ! -d "$dir" ]; then
        echo "Warning: Directory '$dir' not found. Skipping."
        continue
    fi

    for ll_file in "$dir"/*.ll; do
        if [ ! -e "$ll_file" ]; then
            continue
        fi

        filename=$(basename "$ll_file")
        echo -n "Testing: $filename ... "

        # ---------------------------------------------------------
        # 1. Run SC Model
        # ---------------------------------------------------------
        echo ">>> Testing: $ll_file (Model: SC)" >> "$LOG_FILE"
        
        sc_output=$(timeout 5s nidhugg --sc "$ll_file" 2>&1)
        sc_status=$?
        
        echo "$sc_output" >> "$LOG_FILE"
        echo -e "\n------------------------------------------------\n" >> "$LOG_FILE"

        if echo "$sc_output" | grep -q "No errors were detected"; then
            sc_res="Safe"
        elif echo "$sc_output" | grep -q "Error detected:"; then
            sc_res="Bug Detected"
        elif echo "$sc_output" | grep -q "Unsupported external function"; then
            sc_res="Tool Error"
        elif [ $sc_status -eq 124 ]; then
            sc_res="Timeout"
        else
            sc_res="Exception"
        fi

        # ---------------------------------------------------------
        # 2. Run PSO Model
        # ---------------------------------------------------------
        echo ">>> Testing: $ll_file (Model: PSO)" >> "$LOG_FILE"
        
        pso_output=$(timeout 5s nidhugg --pso "$ll_file" 2>&1)
        pso_status=$?
        
        echo "$pso_output" >> "$LOG_FILE"
        echo -e "\n------------------------------------------------\n" >> "$LOG_FILE"

        if echo "$pso_output" | grep -q "No errors were detected"; then
            pso_res="Safe"
        elif echo "$pso_output" | grep -q "Error detected:"; then
            pso_res="Bug Detected"
        elif echo "$pso_output" | grep -q "Unsupported external function"; then
            pso_res="Tool Error"
        elif [ $pso_status -eq 124 ]; then
            pso_res="Timeout"
        else
            pso_res="Exception"
        fi

        # Print concise result to terminal
        echo "SC: $sc_res | PSO: $pso_res"

        # Write to CSV
        echo "$dir,$filename,$sc_res,$pso_res" >> "$SUMMARY_FILE"
    done
done

echo "Testing complete. Summary saved to $SUMMARY_FILE."