#!/bin/bash
SIZES=(10 100 1000 10000 100000 1000000 10000000 100000000)
POLICIES=(0 1)
THREADS=(2 4 8)
RUNS=20

PROGRAM=./ex3
RESULTS_FILE=result.txt


# καθάρισε το αρχείο αποτελεσμάτων
echo "Benchmark Results" > "$RESULTS_FILE"
echo "=================" >> "$RESULTS_FILE"
echo "" >> "$RESULTS_FILE"

for SIZE in "${SIZES[@]}"; do
    echo "Size: $SIZE" >> "$RESULTS_FILE"
    echo "-----------------" >> "$RESULTS_FILE"

    for POLICY in "${POLICIES[@]}"; do

        # αν policy = 0, τρέχουμε ΜΟΝΟ με threads = 2
        if [ "$POLICY" -eq 0 ]; then
            THREAD_LIST=(2)
        else
            THREAD_LIST=("${THREADS[@]}")
        fi

        for T in "${THREAD_LIST[@]}"; do
            total_time=0
            valid_runs=0

            for ((i=1; i<=RUNS; i++)); do
                output=$($PROGRAM "$SIZE" "$POLICY" "$T")

                status=$(echo "$output" | head -n 1)
                time=$(echo "$output" | tail -n 1)

                if [ "$status" != "Sorted" ]; then
                    echo "ERROR: Not sorted (size=$SIZE policy=$POLICY threads=$T run=$i)" >&2
                    continue
                fi

                total_time=$(echo "$total_time + $time" | bc -l)
                valid_runs=$((valid_runs + 1))
            done

            if [ "$valid_runs" -gt 0 ]; then
                avg_time=$(echo "$total_time $valid_runs" | awk '{ printf "%.6f", $1 / $2 }')

                printf "Policy: %d | Threads: %d | Avg Time: %s sec\n" \
                    "$POLICY" "$T" "$avg_time" >> "$RESULTS_FILE"
            else
                printf "Policy: %d | Threads: %d | Avg Time: FAILED\n" \
                    "$POLICY" "$T" >> "$RESULTS_FILE"
            fi
        done
    done

    echo "" >> "$RESULTS_FILE"
done

echo "Done. Results saved in $RESULTS_FILE"