#include <omp.h>

#define _POSIX_C_SOURCE 199309L
#include <time.h>

#include "fun.h"

int main(int argc, char **argv){

    security_user_input(argc, argv); // Make sure user give right input

    int N = atoi(argv[1]), threads = atoi(argv[3]);
    _policy policy = atoi(argv[2]);

    struct timespec tic, toc;

    int *a = malloc(sizeof(int) * N);
    if(a == NULL){
        HANDLE_ERROR("main -> malloc 1")
    }

    int *aux = malloc(sizeof(int) * N);
    if(aux == NULL){
        HANDLE_ERROR("main -> malloc 2")
    }

    init_random_array(a, N);

    clock_gettime(CLOCK_MONOTONIC, &tic);

    switch(policy){

        case serial:
            merge_sort(a, aux, 0, N-1);
            break;
        
        case parallel:
            #pragma omp parallel num_threads(threads)
            {
                #pragma omp single nowait
                {
                    parallel_merge_sort(a, aux, 0, N-1);
                } 
            }
            break;
    }

    clock_gettime(CLOCK_MONOTONIC, &toc);

    (is_sorted(a, N))
        ? printf("Sorted\n")
        : printf("Not sorted\n");

    printf("%.6f\n", (toc.tv_sec - tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1e9);

    free(a);
    free(aux);

    return 0;
}