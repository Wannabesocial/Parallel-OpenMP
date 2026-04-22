#include <omp.h>
#include <string.h>
#include "fun.h"



// ----------------- GENERAL PERPUSE ------------------
void init_random_array(int *a, const int N){

    srand(SEED);

    for(int i = 0; i < N; i++){
        a[i] = rand(); 
    }
}

bool is_sorted(const int *a, const int N){

    for(int i = 0; i < N-1; i++){

        if(a[i] > a[i+1]){
            return false;
        }
    }

    return true;
}

void security_user_input(int argc, char **argv){

    int N, threads_num , policy;

    if(argc != 4){ // Problem: Not enough arguments
        HANDLE_ERROR("Usage: ./<executable> <array size> <policy number> <threads number>");
    }

    N = atoi(argv[1]), policy = atoi(argv[2]), threads_num = atoi(argv[3]);

    if(N < 0 || policy < 0 || threads_num < 0){ // Problem: Negative values
        HANDLE_ERROR("You must not give negatives values");
    }

    if(policy > 1){ // Problem: No such policy. Policy range [0,1]
        HANDLE_ERROR("No such Policy. Give Policy number 0-1\n" "0: serial\n" "1: parallel\n");
    }

    if(N == 0 || (threads_num == 0 && policy == parallel)){ // Problem: Zero values
        HANDLE_ERROR("Program does not have any meaning with zero Loops or Threads");
    }
}


// ----------------- SERIAL ------------------

void merge(int *a, int *aux, const int start_a,
    const int end_a, const int end_b)
{
    int start_b = end_a + 1;

    // Temp save first array values [start_a, end_a]
    memcpy(&aux[start_a], &a[start_a], sizeof(int) * (end_a-start_a+1));

    // Temp save secont array values [end_a+1, end_b]
    memcpy(&aux[start_b], &a[start_b], sizeof(int) * (end_b-start_b+1));

    int i = start_a, j = start_b;

    for(int k = start_a; k <= end_b; k++){

        // No more values in "first" array
        if(i == end_a + 1){
            a[k] = aux[j++];
            continue;
        }

        // No more values in "secont" array
        if(j == end_b + 1){
            a[k] = aux[i++];
            continue;
        }

        // Pick the lower one
        a[k] = (aux[i] <= aux[j]) ? aux[i++] : aux[j++];
    }
}

void merge_sort(int *a, int *aux, 
    const int start, const int end)
{
    int mid = (start + end) / 2;

    // Already soarted
    if(end - start <= 0) return;

    merge_sort(a, aux, start, mid);
    merge_sort(a, aux, mid + 1, end);
    merge(a, aux, start, mid, end);
}


// ----------------- PARALLEL ------------------

void parallel_merge_sort(int *a, int *aux, 
    const int start, const int end)
{

    int mid = (start + end) / 2;
    int size = end - start + 1;

    // Already soarted -> array with size 1
    if(size <= 1) return;

    #pragma omp task if(size > THRESHOLD) \
        firstprivate(start) firstprivate(mid) 
    parallel_merge_sort(a, aux, start, mid);

    #pragma omp task if(size > THRESHOLD) \
        firstprivate(mid) firstprivate(end) 
    parallel_merge_sort(a, aux, mid + 1, end);    

    #pragma omp taskwait
    merge(a, aux, start, mid, end);
}
