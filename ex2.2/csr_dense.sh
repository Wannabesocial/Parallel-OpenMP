#!/bin/bash

SIZES=10000
SPARSITIES=97
THREADS=4
ITERS=10
MULTS=(1 5 10 15 20)


EXEC="./ex2"
OUTPUT_CSV="csr_dense.csv"

echo "Size,Sparsity,Mult,Threads,Avg_Total_CSR_Time,Avg_Sparse_MV_Time,STATUS" > $OUTPUT_CSV

for mult in "${MULTS[@]}"; do
            
    total_csr_sum=0
    total_sparse_mv_sum=0
    final_status="UNKNOWN"

    # echo "Running: Size=$size, Sparsity=$sparsity, Threads=$thread"
    echo "Mult: $mult"

    for ((i=1; i<=ITERS; i++)); do

        echo "Loop [$i/$ITERS]....."

         # Εκτέλεση
        output=$($EXEC $SIZES $SPARSITIES $mult $THREADS)
                
        # Χρήση του : ως διαχωριστικό (-F':') για να πάρουμε καθαρά τον αριθμό
        create_time=$(echo "$output" | grep "Parallel CSR create time" | awk -F': ' '{print $2}' | xargs)
        csr_prod_time=$(echo "$output" | grep "Parallel CSR-Vector product time" | awk -F': ' '{print $2}' | xargs)
        sparse_mv_time=$(echo "$output" | grep "Sparese Matrix-Vector product time" | awk -F': ' '{print $2}' | xargs)

        # Διόρθωση στο status (χρήση $output αντί για $result)
        current_status=$(echo "$output" | grep "CSR-Vector and Sparse-Matrix product" | awk -F': ' '{print $2}' | xargs)
        [ ! -z "$current_status" ] && final_status=$current_status

        # Έλεγχος αν οι τιμές είναι αριθμοί πριν την bc
        if [[ -z "$create_time" || -z "$csr_prod_time" ]]; then
            create_time=0
            csr_prod_time=0
        fi
        if [[ -z "$sparse_mv_time" ]]; then
            sparse_mv_time=0
        fi

        # Υπολογισμοί με bc
        combined_csr=$(echo "$create_time + $csr_prod_time" | bc -l)
        total_csr_sum=$(echo "$total_csr_sum + $combined_csr" | bc -l)
        total_sparse_mv_sum=$(echo "$total_sparse_mv_sum + $sparse_mv_time" | bc -l)
    done

    # Υπολογισμός Μέσου Όρου
    avg_csr=$(echo "scale=8; $total_csr_sum / $ITERS" | bc -l)
    avg_sparse=$(echo "scale=8; $total_sparse_mv_sum / $ITERS" | bc -l)

    echo "$SIZES,$SPARSITIES,$mult,$THREADS,$avg_csr,$avg_sparse,$final_status" >> $OUTPUT_CSV
done

echo "Done! Results saved in $OUTPUT_CSV"

# for size in "${SIZES[@]}"; do
#     for sparsity in "${SPARSITIES[@]}"; do
#         for thread in "${THREADS[@]}"; do
            
#             total_csr_sum=0
#             total_sparse_mv_sum=0
#             final_status="UNKNOWN"

#             echo "Running: Size=$size, Sparsity=$sparsity, Threads=$thread"

#             for ((i=1; i<=ITERS; i++)); do

#                 echo "Loop [$i/$ITERS]....."

#                 # Εκτέλεση
#                 output=$($EXEC $size $sparsity 1 $thread)
                
#                 # Χρήση του : ως διαχωριστικό (-F':') για να πάρουμε καθαρά τον αριθμό
#                 create_time=$(echo "$output" | grep "Parallel CSR create time" | awk -F': ' '{print $2}' | xargs)
#                 csr_prod_time=$(echo "$output" | grep "Parallel CSR-Vector product time" | awk -F': ' '{print $2}' | xargs)
#                 sparse_mv_time=$(echo "$output" | grep "Sparese Matrix-Vector product time" | awk -F': ' '{print $2}' | xargs)

#                 # Διόρθωση στο status (χρήση $output αντί για $result)
#                 current_status=$(echo "$output" | grep "CSR-Vector and Sparse-Matrix product" | awk -F': ' '{print $2}' | xargs)
#                 [ ! -z "$current_status" ] && final_status=$current_status

#                 # Έλεγχος αν οι τιμές είναι αριθμοί πριν την bc
#                 if [[ -z "$create_time" || -z "$csr_prod_time" ]]; then
#                     create_time=0
#                     csr_prod_time=0
#                 fi
#                 if [[ -z "$sparse_mv_time" ]]; then
#                     sparse_mv_time=0
#                 fi

#                 # Υπολογισμοί με bc
#                 combined_csr=$(echo "$create_time + $csr_prod_time" | bc -l)
#                 total_csr_sum=$(echo "$total_csr_sum + $combined_csr" | bc -l)
#                 total_sparse_mv_sum=$(echo "$total_sparse_mv_sum + $sparse_mv_time" | bc -l)
#             done

#             # Υπολογισμός Μέσου Όρου
#             avg_csr=$(echo "scale=8; $total_csr_sum / $ITERS" | bc -l)
#             avg_sparse=$(echo "scale=8; $total_sparse_mv_sum / $ITERS" | bc -l)

#             echo "$size,$sparsity,$thread,$avg_csr,$avg_sparse,$final_status" >> $OUTPUT_CSV
#         done
#     done
# done

# echo "Done! Results saved in $OUTPUT_CSV"