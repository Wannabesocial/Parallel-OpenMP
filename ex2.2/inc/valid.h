/**
 *  Validation Functions
 */

#ifndef VALID_H 
#define VALID_H

#include "structs.h"
#include <stdbool.h>

/* Serial VS Parallel. Create Compressed Sparse Row */
bool valid_csr(const _csr *csr_s, const _csr *csr_p);

/* Compare broduct of Ax , Matrix, Vector. */
bool valid_vector(const _vector *test, const _vector *baseline);

/* Valid user input. Command Line Arguments */
void valid_arguments(const int argc, const char **argv);

#endif