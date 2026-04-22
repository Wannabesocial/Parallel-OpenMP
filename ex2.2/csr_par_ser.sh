#!/bin/bash

# Ρυθμίσεις
EXECUTABLE="./ex2" # Αντικατάστησε με το όνομα του αρχείου σου
OUTPUT_FILE="CSR_serial-parallel.csv"
RUNS=10
SIZES=(10000)
THREADS=(1 2 4 8)
SPARSITY=0
MULT=15

# Επικεφαλίδα του CSV
echo "Size,Threads,Sparsity,Avg_Parallel_CSR_Create,Avg_Serial_CSR_Create,Avg_Parallel_Prod,Avg_Serial_Prod,CSR_Status,Prod_Status" > "$OUTPUT_FILE"

echo "Έναρξη μετρήσεων..."

for size in "${SIZES[@]}"; do
    for t in "${THREADS[@]}"; do
        echo "Running: Size=$size, Threads=$t..."

        # Προσωρινές μεταβλητές για το άθροισμα των χρόνων
        sum_p_create=0
        sum_s_create=0
        sum_p_prod=0
        sum_s_prod=0
        
        # Για τα Status κρατάμε την τελευταία τιμή (υποθέτοντας ότι είναι σταθερά)
        csr_status="UNKNOWN"
        prod_status="UNKNOWN"

        for ((i=1; i<=RUNS; i++)); do
            # Εκτέλεση προγράμματος και αποθήκευση εξόδου
            result=$($EXECUTABLE $size $SPARSITY $MULT $t)

            # Φιλτράρισμα τιμών με awk
            p_create=$(echo "$result" | grep "Parallel CSR create time" | awk '{print $5}')
            s_create=$(echo "$result" | grep "Serial CSR create time" | awk '{print $5}')
            p_prod=$(echo "$result" | grep "Parallel CSR-Vector product time" | awk '{print $5}')
            s_prod=$(echo "$result" | grep "Serial CSR-Vector product time" | awk '{print $5}')
            
            # Status (SUCCESS/FAILURE)
            csr_status=$(echo "$result" | grep "CSR creation:" | awk '{print $3}')
            prod_status=$(echo "$result" | grep "CSR-Vector product:" | awk '{print $3}')

            # Πρόσθεση στο σύνολο (χρήση bc για floating point)
            sum_p_create=$(echo "$sum_p_create + $p_create" | bc -l)
            sum_s_create=$(echo "$sum_s_create + $s_create" | bc -l)
            sum_p_prod=$(echo "$sum_p_prod + $p_prod" | bc -l)
            sum_s_prod=$(echo "$sum_s_prod + $s_prod" | bc -l)
        done

        # Υπολογισμός Μέσου Όρου (ΜΟ)
        avg_p_create=$(echo "scale=8; $sum_p_create / $RUNS" | bc -l)
        avg_s_create=$(echo "scale=8; $sum_s_create / $RUNS" | bc -l)
        avg_p_prod=$(echo "scale=8; $sum_p_prod / $RUNS" | bc -l)
        avg_s_prod=$(echo "scale=8; $sum_s_prod / $RUNS" | bc -l)

        # Εγγραφή στο αρχείο CSV
        echo "$size,$t,$SPARSITY,$avg_p_create,$avg_s_create,$avg_p_prod,$avg_s_prod,$csr_status,$prod_status" >> "$OUTPUT_FILE"
    done
done

echo "Οι μετρήσεις ολοκληρώθηκαν. Τα αποτελέσματα βρίσκονται στο $OUTPUT_FILE"