#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "serial.h"
#include "parallel.h"
#include "valid.h"

int main(int argc, char **argv){

    valid_arguments(argc, (const char **)argv); // Make sure user give right input

    struct timespec tic, toc;
    int size, mult, threads;
    float sparsity;
    _sparse_matrix *spm;
    _vector *vec, *vec_csr_p, *vec_spm_p;
    _vector *vec_csr_s, *vec_spm_s;
    _csr *csr_parallel, *csr_serial;

    size = atoi(argv[1]), sparsity = atoi(argv[2]) / 100.0f;
    mult = atoi(argv[3]), threads = atoi(argv[4]);

    // Create random Vector and Matrix with respect to "sparsity"
    srand(SEED);
    spm = spm_create(size, sparsity);
    vec = vec_create(size);

    // Create Parallel Compressed Sparse Row
    clock_gettime(CLOCK_MONOTONIC, &tic);
    csr_parallel = csr_create_struct(size, sparsity);
    p_csr_create(spm, csr_parallel, threads);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Parallel CSR create time: %.6f\n", (toc.tv_sec - tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1e9);


    // Matrix-Vector Parallel product with CSR, multiple times
    clock_gettime(CLOCK_MONOTONIC, &tic);
    vec_csr_p = p_times_matrix_vec_prod_csr(csr_parallel, vec, mult, threads);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Parallel CSR-Vector product time: %.6f\n", (toc.tv_sec - tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1e9);


    // Matrix-Vector Parallel product with Sparse Matrix, multiple times
    clock_gettime(CLOCK_MONOTONIC, &tic);
    vec_spm_p = p_times_matrix_vec_prod_spm(spm, vec, mult, threads);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Parallel Sparese Matrix-Vector product time: %.6f\n", (toc.tv_sec - tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1e9);


    // Create Serial Compressed Sparse Row
    clock_gettime(CLOCK_MONOTONIC, &tic);
    csr_serial = csr_create_struct(size, sparsity);
    s_csr_create(spm, csr_serial);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Serial CSR create time: %.6f\n", (toc.tv_sec - tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1e9);


    // Matrix-Vector Serial product with CSR, multiple times
    clock_gettime(CLOCK_MONOTONIC, &tic);
    vec_csr_s = s_times_matrix_vec_prod_csr(csr_serial, vec, mult);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Serial CSR-Vector product time: %.6f\n", (toc.tv_sec - tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1e9);


    // Matrix-Vector Serial product with Sparse Matrix, multiple times
    clock_gettime(CLOCK_MONOTONIC, &tic);
    vec_spm_s = s_times_matrix_vec_prod_spm(spm, vec, mult);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Serial Sparese Matrix-Vector product time: %.6f\n", (toc.tv_sec - tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1e9);


    // Parallel and Serial has same result
    printf("---------------------------------------\n");
    printf("CSR-Vector and Sparse-Matrix product: ");
    valid_vector(vec_csr_p, vec_spm_p) ? printf("SUCCESS\n") : printf("FAIL\n");

    printf("CSR creation: ");
    valid_csr(csr_serial, csr_parallel) ? printf("SUCCESS\n") : printf("FAIL\n");

    printf("CSR-Vector product: ");
    valid_vector(vec_csr_p, vec_csr_s) ? printf("SUCCESS\n") : printf("FAIL\n");

    printf("Spare Martix-Vector product: ");
    valid_vector(vec_spm_p, vec_spm_s) ? printf("SUCCESS\n") : printf("FAIL\n");

    spm_display(spm);
    vec_display(vec);

    printf("Print result\n");
    vec_display(vec_csr_p);

    // Clean up
    spm_destroy(spm);
    vec_destroy(vec);
    vec_destroy(vec_csr_p);
    vec_destroy(vec_spm_p);
    vec_destroy(vec_csr_s);
    vec_destroy(vec_spm_s);
    csr_destroy_struct(csr_parallel);
    csr_destroy_struct(csr_serial);

    return 0;
}
