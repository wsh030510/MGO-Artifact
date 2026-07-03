#!/bin/bash

REPORT_FILE="genmc_evaluation_report.csv"
echo "Bug_ID,File_Name,Relaxed_Result,SC_Result,Conclusion" > $REPORT_FILE

echo "Starting automated differential testing..."

for file in *_genmc.c; do
    if [ ! -f "$file" ]; then
        echo "No matching test files found."
        break
    fi

    # Use -n to keep the output on a single line for conciseness
    echo -n "Testing: $file ... "

    # Step 1: Run Relaxed model
    relax_output=$(genmc "$file" 2>&1)
    
    if echo "$relax_output" | grep -iq "violation"; then
        r_str="FAILED (Assertion)"
        r_flag="FAILED"
    elif echo "$relax_output" | grep -iq "race"; then
        r_str="FAILED (Data Race)"
        r_flag="FAILED"
    elif echo "$relax_output" | grep -iq "No errors"; then
        r_str="SUCCESS (No Bug)"
        r_flag="SUCCESS"
    else
        r_str="ERROR (Unknown Output)"
        r_flag="ERROR"
        echo "==== $file (Relaxed) ====" >> genmc_debug.log
        echo "$relax_output" >> genmc_debug.log
    fi

    # Step 2: Generate and run SC model
    temp_sc_file="temp_sc_${file}"
    sed 's/memory_order_relaxed/memory_order_seq_cst/g' "$file" > "$temp_sc_file"

    sc_output=$(genmc "$temp_sc_file" 2>&1)
    
    if echo "$sc_output" | grep -iq "violation"; then
        s_str="FAILED (Assertion)"
        s_flag="FAILED"
    elif echo "$sc_output" | grep -iq "race"; then
        s_str="FAILED (Data Race)"
        s_flag="FAILED"
    elif echo "$sc_output" | grep -iq "No errors"; then
        s_str="SUCCESS (Safe)"
        s_flag="SUCCESS"
    else
        s_str="ERROR (Unknown Output)"
        s_flag="ERROR"
        echo "==== $file (SC) ====" >> genmc_debug.log
        echo "$sc_output" >> genmc_debug.log
    fi

    rm -f "$temp_sc_file"

    # Step 3: Determine conclusion
    conclusion="Unknown"
    
    if [ "$r_flag" = "FAILED" ] && [ "$s_flag" = "SUCCESS" ]; then
        conclusion="Confirmed WMM Bug"
    elif [ "$r_flag" = "FAILED" ] && [ "$s_flag" = "FAILED" ]; then
        conclusion="Logic Bug"
    elif [ "$r_flag" = "SUCCESS" ]; then
        conclusion="False Negative"
    else
        conclusion="Error (Check genmc_debug.log)"
    fi

    # Print conclusion to complete the line
    echo "$conclusion"

    # Write to CSV
    echo "N/A,$file,$r_str,$s_str,$conclusion" >> $REPORT_FILE
done

echo "Testing complete. Results saved to $REPORT_FILE."