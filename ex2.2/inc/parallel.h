/**
 * Parallel Algorithms Implementation
 */

#ifndef PARALLEL_H
#define PARALLEL_H

#include "structs.h"
#include <stdbool.h>

/* Create Compressed Sparse Row base on a Sparse Matrix */
void p_csr_create(const _sparse_matrix *spm, const _csr *csr, const char threads);

/* 
Mutliple times Matrix Vector product.With "Compressed Sparse Row".
Return the result
*/
_vector *p_times_matrix_vec_prod_csr(const _csr *csr, const _vector *vec, char times, char threads);

/*
Mutliple times Matrix Vector product. With "Sparse Matrix".
Return the result
*/
_vector *p_times_matrix_vec_prod_spm(const _sparse_matrix *spm, const _vector *vec, char times, char threads);

#endif