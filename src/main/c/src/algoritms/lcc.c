/*
 * LCC algorithm implementation in GraphBLAS.
 */

#include <time.h>
#include <GraphBLAS.h>

#include "utils.h"
#include "graphio.h"

// Z = f(x)
void calculateCombinations(void *z, const void *x) {
    double xd = *(double *) x;
    double *zd = (double *) z;
    (*zd) = ((xd) * (xd - 1));
};

void lcc_dir(BenchmarkParameters benchmarkParameters) {
    GrB_init(GrB_NONBLOCKING);

    GrB_Matrix A;
    ReadMatrix(benchmarkParameters, &A);

    // Variable required by the OK macro
    unsigned int info;

    // The size of Adj
    unsigned long n;

    // Get the number of vertices, or size of the adjacency matrix.
    OK(GrB_Matrix_nrows(&n, A));
    WriteOutDebugMatrix("A", A);

    printf("Processing starts at: %lu\n", time(NULL));

    /*
     * Create unidirected C matrix
     */

    GrB_Matrix At;
    OK(GrB_Matrix_new(&At, GrB_FP64, n, n));
    OK(GrB_transpose(At, NULL, NULL, A, NULL));

    GrB_Matrix C;
    OK(GrB_Matrix_new(&C, GrB_FP64, n, n));
    OK(GrB_eWiseAdd(C, NULL, NULL, GxB_LOR_BOOL, A, At, NULL));
    WriteOutDebugMatrix("C", C);

    /*
     * Calculate A derivatives
     */

    // Row sum of A
    GrB_Vector Cr;
    OK(GrB_Vector_new(&Cr, GrB_FP64, n))
    OK(GrB_reduce(Cr, NULL, NULL, GxB_PLUS_FP64_MONOID, C, NULL))
    WriteOutDebugVector("Cr", Cr);

    /*
     * Calculate wedges
     */

    // Create an unary operator for calculating combinations

    GrB_UnaryOp combinationOp;
    GrB_UnaryOp_new(&combinationOp, &calculateCombinations, GrB_FP64, GrB_FP64);

    // Create vector W for containing number of wedges per vertex
    GrB_Vector W;
    OK(GrB_Vector_new(&W, GrB_FP64, n))
    OK(GrB_apply(W, NULL, NULL, combinationOp, Cr, NULL))
    WriteOutDebugVector("W", W);

    /*
     * Calculate triangles
     */

    // C*A matrix
    GrB_Matrix CA;
    OK(GrB_Matrix_new(&CA, GrB_FP64, n, n))
    OK(GrB_mxm(CA, C, NULL, GxB_PLUS_TIMES_FP64, C, A, NULL))
    WriteOutDebugMatrix("CA", CA);

    // (C*A)*C matrix with element wise multiplication between A2*A
    GrB_Matrix CAC;
    OK(GrB_Matrix_new(&CAC, GrB_FP64, n, n))
    OK(GrB_eWiseMult(CAC, NULL, NULL, GxB_TIMES_FP64_MONOID, CA, C, NULL))
    WriteOutDebugMatrix("CAC", CAC);

    // Determine triangles by (C*A)*C row sum
    GrB_Vector Tr;
    OK(GrB_Vector_new(&Tr, GrB_FP64, n))
    OK(GrB_reduce(Tr, NULL, NULL, GxB_PLUS_FP64_MONOID, CAC, NULL))
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

    printf("Processing ends at: %lu\n", time(NULL));

    WriteOutResult(benchmarkParameters, Cr, LCC);

    GrB_free(&combinationOp);
    GrB_free(&A);
    GrB_free(&At);
    GrB_free(&CA);
    GrB_free(&CAC);
    GrB_free(&Cr);
    GrB_free(&W);
    GrB_free(&Tr);
    GrB_free(&lccDivMonoid);
    GrB_free(&LCC);

#ifdef PRINT_RESULT
    WriteOutDebugMatrix("A3", A3);
#endif

    GrB_finalize();
}

void lcc_undir(BenchmarkParameters benchmarkParameters) {
    GrB_init(GrB_NONBLOCKING);

    GrB_Matrix A;
    ReadMatrix(benchmarkParameters, &A);

    // Variable required by the OK macro
    unsigned int info;

    // The size of Adj
    unsigned long n;

    // Get the number of vertices, or size of the adjacency matrix.
    OK(GrB_Matrix_nrows(&n, A));
    WriteOutDebugMatrix("A", A);

    printf("Processing starts at: %lu\n", time(NULL));

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

    // Create vector W for containing number of wedges per vertex
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
    GrB_Matrix A2A;
    OK(GrB_Matrix_new(&A2A, GrB_FP64, n, n))
    OK(GrB_eWiseMult(A2A, NULL, NULL, GxB_TIMES_FP64_MONOID, A2, A, NULL))
    WriteOutDebugMatrix("A2A", A2A);

    // Determine triangles by A2A row sum
    GrB_Vector Tr;
    OK(GrB_Vector_new(&Tr, GrB_FP64, n))
    OK(GrB_reduce(Tr, NULL, NULL, GxB_PLUS_FP64_MONOID, A2A, NULL))
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

    printf("Processing ends at: %lu\n", time(NULL));

    WriteOutResult(benchmarkParameters, Ar, LCC);

    GrB_free(&combinationOp);
    GrB_free(&A);
    GrB_free(&A2);
    GrB_free(&A2A);
    GrB_free(&Ar);
    GrB_free(&W);
    GrB_free(&Tr);
    GrB_free(&lccDivMonoid);
    GrB_free(&LCC);

#ifdef PRINT_RESULT
    WriteOutDebugMatrix("A2A", A2A);
#endif

    GrB_finalize();
}

int main(int argc, char** argv) {
    BenchmarkParameters benchmarkParameters = ParseCommandLineParameters(argc, argv);
    if (benchmarkParameters.directed) {
        lcc_dir(benchmarkParameters);
    } else {
        lcc_undir(benchmarkParameters);
    }

}