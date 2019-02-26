/*
 * LCC algorithm implementation in GraphBLAS.
 */

// Controls if debug information should be printed
//#define VERBOSE
// Controls if the results should be displayed
//#define PRINT_RESULT

// macro used by OK(...) to free workspace if an error occurs
#define FREE_ALL \
    GrB_free(&A); \
    GrB_free(&A2); \
    GrB_free(&A3); \

#include <GraphBLAS.h>
#include "utils.h"

void WriteOutResult(GrB_Vector vector) {
    GrB_Index nvals;
    GrB_Vector_nvals(&nvals, vector);

    for (size_t i = 0; i < nvals; i++) {
        double x;
        GrB_Vector_extractElement(&x, vector, i);

        printf("%lu %f\n", i+1, x);
    }
}

void WriteOutDebugMatrix(const char *title, GrB_Matrix result) {
    GrB_Index size;
    GrB_Matrix_nrows(&size, result);
    unsigned int element;

    printf("%s:\n", title);
    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            GrB_Info info = GrB_Matrix_extractElement(&element, result, i, j);

            if (info == GrB_SUCCESS)
            {
                printf("%u ", element);
            }
            else if (info == GrB_NO_VALUE)
            {
                // It is up to the user to determine what 'no value'
                // means.  It depends on the semiring used.
                //printf(" [no value] ");
                printf("%u ", 0);
            }
            else
            {
                printf ("Error! %s\n", GrB_error ()) ;
            }

        }
        printf("\n");
    }
}

void WriteOutDebugVector(const char *title, GrB_Vector result) {
    GrB_Index size;
    GrB_Vector_size(&size, result);
    unsigned int element;

    printf("%s:\n", title);
    for(unsigned int i = 0; i < size; i++) {
        GrB_Info info = GrB_Vector_extractElement(&element, result, i);

        if (info == GrB_SUCCESS)
        {
            //printf("%lf ", element);
            printf("%u ", element);
        }
        else if (info == GrB_NO_VALUE)
        {
            // It is up to the user to determine what 'no value'
            // means.  It depends on the semiring used.
            //printf(" [no value] ");
            printf("%u ", 0);
        }
        else
        {
            printf ("Error! %s\n", GrB_error ()) ;
        }

    }
    printf("\n");
}

// TODO: Try to clear and resize S instead of reallocating every time.
int main (int argc, char **argv) {
    if (argc != 3) {
        printf("Invalid arguments\n");
        exit(-1);
    }

    GrB_init(GrB_NONBLOCKING);

    // Variable required by the OK macro
    unsigned int info;

    // The source adjacency matrix
    GrB_Matrix A;
    GrB_Matrix A2;
    GrB_Matrix A3;
    // The size of Adj
    unsigned long n;

    // Load the matrix from file
    const char *graph_path = argv[1];
    size_t tuple_count = strtoul(argv[2], NULL, 10);
    printf("Loading...\n");
    OK(FillMatrixFromFile(graph_path, tuple_count, &A));
    printf("Loaded\n");

    // Get the number of vertices, or size of the adjacency matrix.
    OK(GrB_Matrix_nrows(&n, A));
    OK(GrB_Matrix_new(&A2, GrB_FP64, n, n));
    OK(GrB_Matrix_new(&A3, GrB_FP64, n, n));

    printf("A2=A*A\n");
    OK(GrB_mxm(A2, NULL, NULL, GxB_PLUS_TIMES_FP64, A, A, NULL));
    printf("A3=A2*A\n");
    //OK(GrB_mxm(A3, NULL, NULL, GxB_PLUS_TIMES_FP64, A2, A, NULL));


#ifdef PRINT_RESULT
    WriteOutDebugMatrix("A3", A3);
#endif

    // Free other GraphBLAS objects.
    FREE_ALL

    GrB_finalize();
}