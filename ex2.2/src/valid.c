#include "valid.h"

bool valid_csr(const _csr *csr_s, const _csr *csr_p){

    char valid = 0;

    // Diferent sizes
    if(csr_s->data_size != csr_p->data_size)
        return false;

    if(csr_s->ptr_size != csr_p->ptr_size)
        return false;

    #pragma omp parallel num_threads(8)
    {
        char my_valid = 0;

        #pragma omp for 
        for(int i = 0; i < csr_s->data_size; i++){

            if(csr_s->data[i] == csr_p->data[i])
                continue;

            if(csr_s->indices[i] == csr_p->indices[i])
                continue;

            my_valid = 1;
        }
        
        #pragma omp for
        for(int i = 0; i < csr_s->ptr_size; i++){

            if(csr_s->row_ptr[i] == csr_p->row_ptr[i])
                continue;
            
            my_valid = 1;
        }

        #pragma omp atomic
        valid += my_valid;
    }

    return (valid == 0) ? true : false;
}

bool valid_vector(const _vector *test, const _vector *baseline){

    char valid = 0;

    if(test->size != baseline->size)
        return false;

    #pragma omp parallel num_threads(8)
    {
        char my_valid = 0;

        #pragma omp for
        for(int i = 0; i < test->size; i++){
            
            if(test->array[i] == baseline->array[i])
                continue;

            my_valid = 1;
        }

        #pragma omp atomic
        valid += my_valid;
    }
        
    return (valid == 0) ? true : false;
}

void valid_arguments(const int argc, const char **argv){

    int size, sparsity, mult, threads;

    // Problem: Not enough arguments
    if(argc != 5){
        HANDLE_ERROR("Usage: ./<executable> <array size> <sparsity> <mult times> <threads number>");
    }

    size = atoi(argv[1]), sparsity = atoi(argv[2]);
    mult = atoi(argv[3]), threads = atoi(argv[4]);

    // Problem: Out of range <array size>
    if(size <= 0 || size > 10000){
        HANDLE_ERROR("Out of range <array size>. Range [1, 10^4]");
    }

    // Problem: Out of range <sparsity>
    if(sparsity < 0 || sparsity > 99){
        HANDLE_ERROR("Out of range <sparsity>. Range [0%, 99%]");
    }

    // Problem: Out of range <mult times>
    if(mult < 1 || mult > 20){
        HANDLE_ERROR("Out of range <mult times>. Range [1, 20]");
    }

    // Problem: Out of range <threads number>
    if(threads < 0 || threads > 8){
        HANDLE_ERROR("Out of range <threads number>. Range [0, 8]\n" "linux02.di.uoa.gr has 8 threads");
    }
}
