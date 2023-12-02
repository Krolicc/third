#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

typedef struct {
    double* coords;
    int size;
} Vector;

typedef double (*Norm_func)(Vector*, void*);

double infinity_norm(Vector* vec, void* param) 
{
    double max_coord = 0;
    for (int i = 0; i < vec->size; i++) 
    {
        double coord = fabs(vec->coords[i]);
        if (coord > max_coord) max_coord = coord;
    }
    return max_coord;
}

double p_norm(Vector* vec, void* param) 
{
    double p = *(double*)param;
    double sum = 0;
    for (int i = 0; i < vec->size; i++) 
    {
        sum += pow(fabs(vec->coords[i]), p);
    }
    return pow(sum, 1.0 / p);
}

double A_norm(Vector* vec, void* param) 
{
    double* matrix = (double*)param;
    int n = vec->size;

    double sum = 0;
    for (int i = 0; i < n; i++) 
    {
        double A_x = 0;
        for (int j = 0; j < n; j++) { A_x += vec->coords[j] * matrix[i * n + j]; }
        sum += (fabs(A_x) * vec->coords[i]);
    }
    return sum;
}

void printVector(Vector* vec) 
{
    printf("[");
    for (int i = 0; i < vec->size; i++) 
    {
        printf("%.2f", vec->coords[i]);
        if (i != vec->size - 1) printf(", ");
    }
    printf("]\n");
}

void free_vectors(Vector*** vectors, int vector_count) 
{
    for (int i = 0; i < vector_count; i++) 
    {
        free(vectors[i]);
    }
    free(vectors);
}

Vector*** find_longest_vectors(int num_vectors, int num_norms, ...) 
{
    va_list args;
    va_start(args, num_norms);

    Vector*** longest_per_norm = (Vector***)malloc(num_norms * sizeof(Vector**));
    if (longest_per_norm == NULL) {va_end(args); return NULL;}

    Vector** vectors = (Vector** )malloc(num_vectors * sizeof(Vector*));
    if (vectors == NULL) {va_end(args); free(longest_per_norm); return NULL;}
    for (int i = 0; i < num_vectors; i++) 
    {
        vectors[i] = va_arg(args, Vector*);
    }

    for (int i = 0; i < num_norms; i++) 
    {
        Norm_func func = va_arg(args, Norm_func);
        void* param_for_norm = va_arg(args, void*);

        double max_norm = -1.0;
        Vector** longest_vectors_for_crnt_norm = (Vector**)malloc((num_vectors + 1) * sizeof(Vector*));
        if (longest_vectors_for_crnt_norm == NULL) {free(vectors); free_vectors(longest_per_norm, i); return NULL;}
        int num_longest = 0;

        for (int j = 0; j < num_vectors; j++) 
        {
            Vector* vec = vectors[j];
            double norm = func(vec, param_for_norm);

            if (norm > max_norm) 
            {
                max_norm = norm;
                num_longest = 0;
            }

            if (norm == max_norm) 
            {
                longest_vectors_for_crnt_norm[num_longest] = vec; //copy
                num_longest++;
            }
        }

        longest_vectors_for_crnt_norm[num_longest] = NULL; // add a NULL pointer at the end

        Vector ** ptr = realloc(longest_vectors_for_crnt_norm, (num_longest + 1) * sizeof(Vector*)); // +1 for the NULL pointer
        if (ptr == NULL) {free(vectors); free(longest_vectors_for_crnt_norm); free_vectors(longest_per_norm, i); return NULL;}
        longest_per_norm[i] = ptr;
    }

    va_end(args);
    free(vectors);

    return longest_per_norm;
}

int main() 
{
    Vector v1 = { .coords = (double[]){1, 2, 3}, .size = 3 };
    Vector v2 = { .coords = (double[]){-1, 0, 1}, .size = 3 };
    Vector v3 = { .coords = (double[]){3, 2, 1}, .size = 3 };

    double p = 2.0;

    double matrix[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    Vector*** longest_per_norm = find_longest_vectors(3, 3, &v1, &v2, &v3, &infinity_norm, NULL, &p_norm, &p, &A_norm, matrix);


    if (longest_per_norm == NULL) { printf("Memory allocation problem\n"); return 1;}
    for (int i = 0; i < 3; i++) 
    {
        printf("Norm function %d:\n", i + 1);
        for (int j = 0; longest_per_norm[i][j] != NULL; j++) 
        {
            printf("Vector %d: ", j + 1);
            printVector(longest_per_norm[i][j]);
        }
    }

    // Free the memory
    free_vectors(longest_per_norm, 3);

    return 0;
}

