/**
 * Serial Algorithms Implementation
 */

#ifndef SERIAL_H
#define SERIAL_H

#include "structs.h"

/* Create Compressed Sparse Row base on a Sparse Matrix */
void s_csr_create(const _sparse_matrix *spm, const _csr *csr);

/* 
Mutliple times Matrix Vector product.With "Compressed Sparse Row".
Return the result
*/
_vector *s_times_matrix_vec_prod_spm(const _sparse_matrix *spm, const _vector *vec, const char times);


/*
Mutliple times Matrix Vector product. With "Sparse Matrix".
Return the result
*/
_vector *s_times_matrix_vec_prod_csr(const _csr *csr, const _vector *vec, const char times);

#endif