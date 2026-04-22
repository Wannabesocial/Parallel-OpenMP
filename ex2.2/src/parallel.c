#include "parallel.h"

#include <stdbool.h>
#include <string.h>
#include <omp.h>

void p_csr_create(const _sparse_matrix *spm, const _csr *csr, const char threads){

    const int8_t **array = (const int8_t **)spm->array;
    int size = spm->size;

    csr->row_ptr[0] = 0;

    int *m_d = (int *) malloc(sizeof(int) * threads);
    if(m_d == NULL){
        HANDLE_ERROR("malloc -> p_csr_create");
    }

    #pragma omp parallel num_threads(threads)
    {
        int sum = 0, rank = omp_get_thread_num(), idx;

        #pragma omp for
        for(int i = 0; i < size; i++){

            for(int j = 0; j < size; j++){
                    
                // Skip the zeros elements
                if(array[i][j] == 0)
                    continue;  
                    
                sum++;
            }
        }

        m_d[rank] = sum;
        #pragma omp barrier

        idx = 0;
        for(int i = 0; i < rank; i++){
            idx += m_d[i];
        }
        
        #pragma omp for
        for(int i = 0; i < size; i++){

            int non_zeros = 0;

            for(int j = 0; j < size; j++){

                // Skip the zeros elements
                if(array[i][j] == 0)
                    continue; 
                    
                csr->data[idx] = array[i][j];
                csr->indices[idx] = j;
                idx++;
                non_zeros++;       
            }

            csr->row_ptr[i + 1] = non_zeros;
        }
    }

    // Fix row_ptr, offset
    for(int i = 1; i < csr->ptr_size; i++){
        csr->row_ptr[i] += csr->row_ptr[i - 1];
    }

    free(m_d);
}

_vector *p_times_matrix_vec_prod_csr(const _csr *csr, const _vector *vec, char times, char threads){

    int64_t *temp, c;
    int size = vec->size;
    int start, end;
    _vector *result;

    result = vec_copy(vec);

    temp = (int64_t *) malloc(sizeof(int64_t) * size);
    if(temp == NULL){
        HANDLE_ERROR("malloc -> s_matrix_vec_prod_spm");
    }

    #pragma omp parallel num_threads(threads)
    for(int t = 0; t < times; t++){

        #pragma omp single
        memcpy(temp, result->array, sizeof(int64_t) * size);

        #pragma omp for \
            private(c, start, end)
        // Product calculation
        for(int i = 0; i < size; i++){

            start = csr->row_ptr[i];
            end = csr->row_ptr[i+1];
            c = 0;

            for(int j = start; j < end; j++){
                c += csr->data[j] * result->array[csr->indices[j]];
            }

            temp[i] = c;
        }

        #pragma omp single
        memcpy(result->array, temp, sizeof(int64_t) * size);
    }
    
    free(temp);
    return result;
}

_vector *p_times_matrix_vec_prod_spm(const _sparse_matrix *spm, const _vector *vec, char times, char threads){

    const int8_t **array = (const int8_t **) spm->array;
    int size = spm->size;
    int64_t *temp, c;
    _vector *result;

    result = vec_copy(vec);

    temp = (int64_t *) malloc(sizeof(int64_t) * size);
    if(temp == NULL){
        HANDLE_ERROR("malloc -> s_matrix_vec_prod_spm");
    }

    #pragma omp parallel num_threads(threads)
    for(int t = 0; t < times; t++){
    
        #pragma omp single
        memcpy(temp, result->array, sizeof(int64_t) * size);

        #pragma omp for \
            private(c)
        // Product calculation
        for(int i = 0; i < size; i++){

            c = 0;

            for(int j = 0; j < size; j++){
                c += result->array[j] * array[i][j];
            }

            temp[i] = c;
        }

        #pragma omp single
        memcpy(result->array, temp, sizeof(int64_t) * size);
    }

    free(temp);
    return result;
}
