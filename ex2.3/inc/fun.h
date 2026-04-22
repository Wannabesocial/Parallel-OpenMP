#ifndef FUN_H
#define FUN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SEED 42
#define THRESHOLD 4096 // limit for parallel work

/**
 * Some usefull macros
 */
#define HANDLE_ERROR(msg){ perror(msg); exit(EXIT_FAILURE); }


/**
 * Some usefull functions
 */

typedef enum _policy{serial, parallel} _policy;

/**
 * Security. Make sure the user give input in right order and is valid.
 * Execute with ./<executable> <array size> <policy number> <threads number>
 * Policy:
 *  (0) serial
 *  (1) parallel
**/
void security_user_input(int argc, char **argv);

/*
Initialize a array with random integers --- Const Seed --- 
*/
void init_random_array(int *a, const int N);

/*
Check if a array of integers is sorted
*/
bool is_sorted(const int *a, const int N);



// ----------------- SERIAL ------------------

/*
Merge. Take final array and a helpfull one AUX. Merge into "a"
Both bellow conridates in array "a".
1st array = [start_a, end_a]
2cond array = [end_a+1, end_b]
*/
void merge(int *a, int *aux, const int start_a,
    const int end_a, const int end_b);

/*
MergeSort. The basic devine and quonquer sort algorithm
We working in "a" and "aux" [start, end]
*/
void merge_sort(int *a, int *aux, 
    const int start, const int end);



// ----------------- PARALLEL ------------------
void parallel_merge_sort(int *a, int *aux, 
    const int start, const int end);


#endif