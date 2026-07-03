#!/bin/bash

LOG_FILE="cbmc_detailed.log"
SUMMARY_FILE="cbmc_summary_report.csv"
TIMEOUT_SECS=10

echo "CBMC Detailed Log" > "$LOG_FILE"
echo "Start Time: $(date)" >> "$LOG_FILE"
echo "------------------------------------------------" >> "$LOG_FILE"

echo "File_Name,TSO_Result,PSO_Result,Conclusion" > "$SUMMARY_FILE"

echo "Starting batch CBMC tests (TSO vs PSO) in strict numerical order..."

# Use ls -v to ensure natural numerical sorting (1, 2, ..., 10, 11)
for file in $(ls -v *.c 2>/dev/null); do
    if [ ! -e "$file" ]; then
        echo "No .c files found in current directory."
        break
    fi

    echo -n "Testing: $file ... "

    # ---------------------------------------------------------
    # 1. Run TSO Model
    # ---------------------------------------------------------
    echo ">>> Testing: $file (Model: TSO)" >> "$LOG_FILE"
    
    tso_output=$(timeout $TIMEOUT_SECS cbmc "$file" --mm tso 2>&1)
    tso_status=$?
    
    echo "$tso_output" >> "$LOG_FILE"
    echo -e "\n------------------------------------------------\n" >> "$LOG_FILE"

    if echo "$tso_output" | grep -q "VERIFICATION SUCCESSFUL"; then
        tso_res="Safe"
    elif echo "$tso_output" | grep -q "VERIFICATION FAILED"; then
        tso_res="Bug Detected"
    elif [ $tso_status -eq 124 ]; then
        tso_res="Timeout"
    else
        tso_res="Exception"
    fi

    # ---------------------------------------------------------
    # 2. Run PSO Model
    # ---------------------------------------------------------
    echo ">>> Testing: $file (Model: PSO)" >> "$LOG_FILE"
    
    pso_output=$(timeout $TIMEOUT_SECS cbmc "$file" --mm pso 2>&1)
    pso_status=$?
    
    echo "$pso_output" >> "$LOG_FILE"
    echo -e "\n------------------------------------------------\n" >> "$LOG_FILE"

    if echo "$pso_output" | grep -q "VERIFICATION SUCCESSFUL"; then
        pso_res="Safe"
    elif echo "$pso_output" | grep -q "VERIFICATION FAILED"; then
        pso_res="Bug Detected"
    elif [ $pso_status -eq 124 ]; then
        pso_res="Timeout"
    else
        pso_res="Exception"
    fi

    # ---------------------------------------------------------
    # 3. Determine Weak Memory Bug
    # ---------------------------------------------------------
    conclusion="Unknown"
    
    if [ "$tso_res" = "Safe" ] && [ "$pso_res" = "Bug Detected" ]; then
        conclusion="WMM Bug"
    elif [ "$tso_res" = "Bug Detected" ] && [ "$pso_res" = "Bug Detected" ]; then
        conclusion="Logic Bug"
    elif [ "$tso_res" = "Safe" ] && [ "$pso_res" = "Safe" ]; then
        conclusion="No Bug"
    else
        conclusion="Error/Timeout (Check Log)"
    fi

    echo "TSO: $tso_res | PSO: $pso_res -> $conclusion"

    echo "$file,$tso_res,$pso_res,$conclusion" >> "$SUMMARY_FILE"
done

echo "Testing complete. Summary saved to $SUMMARY_FILE."