/*
 * LCC algorithm implementation in GraphBLAS.
 */

// Controls if debug information should be printed
//#define VERBOSE
// Controls if the results should be displayed
//#define PRINT_RESULT

// macro used by OK(...) to free workspace if an error occurs
#define FREE_ALL \


#include <GraphBLAS.h>
#include "utils.h"

void WriteOutResult(GrB_Vector vector) {
    GrB_Index nvals;
    GrB_Vector_nvals(&nvals, vector);

    for (size_t i = 0; i < nvals; i++) {
        double x;
        GrB_Vector_extractElement(&x, vector, i);

        printf("%lu %f\n", i + 1, x);
    }
}

void WriteOutDebugMatrix(const char *title, GrB_Matrix result) {
    printf("%s:\n", title);
#ifdef VERBOSE
    GrB_Index size;
    GrB_Matrix_nrows(&size, result);
    unsigned int element;

    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            GrB_Info info = GrB_Matrix_extractElement(&element, result, i, j);

            if (info == GrB_SUCCESS) {
                printf("%u ", element);
            } else if (info == GrB_NO_VALUE) {
                // It is up to the user to determine what 'no value'
                // means.  It depends on the semiring used.
                //printf(" [no value] ");
                printf("%u ", 0);
            } else {
                printf("Error! %s\n", GrB_error());
            }

        }
        printf("\n");
    }
#endif
}

void WriteOutDebugVector(const char *title, GrB_Vector result) {
    printf("%s:\n", title);
#ifdef VERBOSE
    GrB_Index size;
    GrB_Vector_size(&size, result);
    double element;

    for (unsigned int i = 0; i < size; i++) {
        GrB_Info info = GrB_Vector_extractElement(&element, result, i);

        if (info == GrB_SUCCESS) {
            printf("%lf ", element);
//            printf("%u ", element);
        } else if (info == GrB_NO_VALUE) {
            // It is up to the user to determine what 'no value'
            // means.  It depends on the semiring used.
            printf(" [no value] ");
//            printf("%u ", 0);
        } else {
            printf("Error! %s\n", GrB_error());
        }

    }
    printf("\n");
#endif
}

// Z = f(x)
void calculateCombinations(void *z, const void *x) {
    double xd = *(double *) x;
    double *zd = (double *) z;
    (*zd) = ((xd) * (xd - 1));
};

// TODO: Try to clear and resize S instead of reallocating every time.
int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Invalid arguments\n");
        exit(-1);
    }

    GrB_init(GrB_NONBLOCKING);

    // Variable required by the OK macro
    unsigned int info;

    // The source adjacency matrix
    GrB_Matrix A;
    // The size of Adj
    unsigned long n;

    // Load the matrix from file
    const char *graph_path = argv[1];
    size_t tuple_count = strtoul(argv[2], NULL, 10);
    printf("Loading...\n");
    OK(FillMatrixFromFile(graph_path, tuple_count, &A))
    printf("Loaded\n");

    // Get the number of vertices, or size of the adjacency matrix.
    OK(GrB_Matrix_nrows(&n, A));
    WriteOutDebugMatrix("A", A);

    /*
     * Calculate A derivatives
     */

    // Row sum of A
    GrB_Vector Ar;
    OK(GrB_Vector_new(&Ar, GrB_FP64, n))
    OK(GrB_reduce(Ar, NULL, NULL, GxB_PLUS_FP64_MONOID, A, NULL))
    WriteOutDebugVector("Ar", Ar);

    /*
     * Calculate wedges
     */

    // Create an unary operator for calculating combinations
    GrB_UnaryOp combinationOp;
    GrB_UnaryOp_new(&combinationOp, &calculateCombinations, GrB_FP64, GrB_FP64);

    // Create vector W for containing number of edges per vertex
    GrB_Vector W;
    OK(GrB_Vector_new(&W, GrB_FP64, n))
    OK(GrB_apply(W, NULL, NULL, combinationOp, Ar, NULL))
    WriteOutDebugVector("W", W);

    /*
     * Calculate triangles
     */

    // A^2 matrix
    GrB_Matrix A2;
    OK(GrB_Matrix_new(&A2, GrB_FP64, n, n))
    OK(GrB_mxm(A2, A, NULL, GxB_PLUS_TIMES_FP64, A, A, NULL))
    WriteOutDebugMatrix("A2", A2);

    // ~A^3 matrix with element wise multiplication between A2*A
    GrB_Matrix A3;
    OK(GrB_Matrix_new(&A3, GrB_FP64, n, n))
    OK(GrB_eWiseMult(A3, NULL, NULL, GxB_TIMES_FP64_MONOID, A2, A, NULL))
    WriteOutDebugMatrix("A3", A3);

    // Determine triangles by A3 row sum
    GrB_Vector Tr;
    OK(GrB_Vector_new(&Tr, GrB_FP64, n))
    OK(GrB_reduce(Tr, NULL, NULL, GxB_PLUS_FP64_MONOID, A3, NULL))
    WriteOutDebugVector("Tr", Tr);

    /*
     * Calculate LCC
     */
    // Create an unary operator for calculating combinations
    GrB_Monoid lccDivMonoid;
    GrB_Monoid_new(&lccDivMonoid, GrB_DIV_FP64, 1.0);

    GrB_Vector LCC;
    OK(GrB_Vector_new(&LCC, GrB_FP64, n))
    OK(GrB_eWiseMult(LCC, NULL, NULL, lccDivMonoid, Tr, W, NULL))
    WriteOutDebugVector("LCC", LCC);

#ifdef PRINT_RESULT
    WriteOutDebugMatrix("A3", A3);
#endif

    // Free other GraphBLAS objects.
    FREE_ALL

    GrB_finalize();
}