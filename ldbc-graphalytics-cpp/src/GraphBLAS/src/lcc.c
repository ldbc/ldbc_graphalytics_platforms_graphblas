/*
 * LCC algorithm implementation in GraphBLAS.
 */

// Controls if debug information should be printed
//#define VERBOSE
// Controls if the results should be displayed
#define PRINT_RESULT

// macro used by OK(...) to free workspace if an error occurs
#define FREE_ALL \
    GrB_free(&A); \
    GrB_free(&v); \
    GrB_free(&v_in_neighbors); \
    GrB_free(&v_out_neighbors); \
    GrB_free(&v_neighbors); \
    GrB_free(&n_neighbors); \
    GrB_free(&n_masked_neighbors); \
    GrB_free(&result);

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

    GrB_Matrix A;
    // Indicator vector to select a vertex
    GrB_Vector v = NULL;
    // Vector containing the out edges from v
    GrB_Vector v_out_neighbors = NULL;
    // Vector containing the in edges from v
    GrB_Vector v_in_neighbors = NULL;
    // Vector containing the in+out edges
    GrB_Vector v_neighbors = NULL;

    // Vector containing the neighbours of N(v) vertices
    GrB_Vector n_neighbors = NULL;
    // Vector containing the neighbours of N(v) vertices
    // but only those which are in N(v)
    GrB_Vector n_masked_neighbors = NULL;

    // The result vector
    GrB_Vector result = NULL;

    // The dimensions of the matrix
    unsigned long n;
    // Contains |N(v)|
    unsigned long neighbor_count;
    // Contains the edges between N(v)
    unsigned long edge_count;

    // Load the matrix from file
    const char *graph_path = argv[1];
    size_t tuple_count = strtoul(argv[2], NULL, 10);
    OK(FillMatrixFromFile(graph_path, tuple_count, &A));

    // Get the number of vertices, or size of the adjacency matrix.
    OK(GrB_Matrix_nrows(&n, A));

    // Initialize the vectors.
    OK(GrB_Vector_new(&v, GrB_UINT64, n));

    OK(GrB_Vector_new(&v_in_neighbors, GrB_UINT64, n));
    OK(GrB_Vector_new(&v_out_neighbors, GrB_UINT64, n));
    OK(GrB_Vector_new(&v_neighbors, GrB_UINT64, n));
    OK(GrB_Vector_new(&n_neighbors, GrB_UINT64, n));
    OK(GrB_Vector_new(&n_masked_neighbors, GrB_UINT64, n));

    OK(GrB_Vector_new(&result, GrB_FP64, n));

    // Iterate through all vertices.
    for (unsigned long i = 0; i < n; i++) {
        printf("Vertex %d\n", i);
        /*
         * Calculate the neighbors of v
         */
        // Set the selector value in v
        OK(GrB_Vector_setElement(v, 1, i));
        // Do a (v*A) to find N_out(v) aka. the outbound edges
        OK(GrB_vxm(v_out_neighbors, NULL, NULL, GxB_LOR_LAND_BOOL, v, A, NULL));
        // Do a (A*v^t) to find N_in(v) aka. the inbound edges
        // TODO: look up if there is any bottleneck multiplying from the right side
        OK(GrB_mxv(v_in_neighbors, NULL, NULL, GxB_LOR_LAND_BOOL, A, v, NULL));
        // Reset the indicator value
        OK(GrB_Vector_setElement(v, 0, i));
        OK(GrB_eWiseAdd(v_neighbors, NULL, NULL, GxB_LOR_BOOL_MONOID, v_out_neighbors, v_in_neighbors, NULL))
        OK(GrB_Vector_reduce_UINT64(&neighbor_count, NULL, GxB_PLUS_UINT64_MONOID, v_neighbors, NULL))

        /*
         * Calculate the neighbours of N(v)
         */
        OK(GrB_vxm(n_neighbors, NULL, NULL, GxB_PLUS_TIMES_UINT64, v_neighbors, A, NULL))
        OK(GrB_eWiseMult(n_masked_neighbors, NULL, NULL, GxB_PLUS_TIMES_UINT64, v_neighbors, n_neighbors, NULL))

        /*
         * Calculate the neighbours of N(v)
         */
        OK(GrB_Vector_reduce_UINT64(&edge_count, NULL, GxB_PLUS_INT64_MONOID, n_masked_neighbors, NULL))

#ifdef VERBOSE
        WriteOutDebugMatrix("A", A);
        WriteOutDebugVector("v_out_neighbors", v_out_neighbors);
        WriteOutDebugVector("v_in_neighbors", v_in_neighbors);
        WriteOutDebugVector("v_neighbors", v_neighbors);
        WriteOutDebugVector("n_neighbors", n_neighbors);
        WriteOutDebugVector("n_masked_neighbors", n_masked_neighbors);
        printf("\n");
#endif

        // Count lcc for the current vertex, then save it into the result vector.
        double lcc;
        if ((int) neighbor_count <= 1) {
            lcc = 0.0;
        } else {
            lcc = (double) edge_count / (neighbor_count * (neighbor_count - 1));
        }

        GrB_Vector_setElement(result, lcc, i);
    }

#ifdef PRINT_RESULT
    WriteOutResult(result);
#endif

    // Free other GraphBLAS objects.
    FREE_ALL

    GrB_finalize();
}