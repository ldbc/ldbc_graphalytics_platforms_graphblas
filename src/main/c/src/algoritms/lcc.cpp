/*
 * LCC algorithm implementation in GraphBLAS.
 */

#include <iostream>
#include <ctime>

extern "C" {
#include <GraphBLAS.h>
}

#include "utils.h"
#include "graphio.h"

// Z = f(x) * f(x) - 1
// This operator calculates the comb(d(v), 2) value.
//
// Note: the 2 division can be skipped, because it
// will be the final division in the LCC calculation
void calculateCombinations(void *z, const void *x) {
    double xd = *(double *) x;
    auto *zd = (double *) z;
    (*zd) = ((xd) * (xd - 1));
};

void lcc_dir(BenchmarkParameters benchmarkParameters) {
    GrB_init(GrB_NONBLOCKING);

    std::cout << "Loading" << std::endl;
    GrB_Matrix A;
    IndexMap mapping = ReadMatrix(benchmarkParameters, A);

    // Variable required by the OK macro
    unsigned int info;

    // The size of Adj
    unsigned long n;

    // Get the number of vertices, or size of the adjacency matrix.
    OK(GrB_Matrix_nrows(&n, A));
    WriteOutDebugMatrix("A", A);

    printf("Processing starts at: %lu\n", time(nullptr));

    /*
     * Create unidirected C matrix, by A (+) At
     * over a boolean binary operator
     */
    GrB_Matrix At;
    OK(GrB_Matrix_new(&At, GrB_FP64, n, n));
    OK(GrB_transpose(At, nullptr, nullptr, A, nullptr));

    GrB_Matrix C;
    OK(GrB_Matrix_new(&C, GrB_FP64, n, n));
    OK(GrB_eWiseAdd_Matrix_BinaryOp(C, nullptr, nullptr, GxB_LOR_BOOL, A, At, nullptr));
    WriteOutDebugMatrix("C", C);

    /*
     * Calculate A derivatives
     */

    // Row sum of A
    GrB_Vector Cr;
    OK(GrB_Vector_new(&Cr, GrB_FP64, n))
    OK(GrB_Matrix_reduce_Monoid(Cr, nullptr, nullptr, GxB_PLUS_FP64_MONOID, C, nullptr))
    WriteOutDebugVector("Cr", Cr);

    /*
     * Calculate wedges
     */

    // Create an unary operator for calculating combinations
    GrB_UnaryOp combinationOp;
    GrB_UnaryOp_new(&combinationOp, &calculateCombinations, GrB_FP64, GrB_FP64);

    // Create vector W for containing number of wedges per vertex
    // Wedges per vertex is calculated by (d(v) * (d(v) - 1)
    GrB_Vector W;
    OK(GrB_Vector_new(&W, GrB_FP64, n))
    OK(GrB_Vector_apply(W, nullptr, nullptr, combinationOp, Cr, nullptr))
    WriteOutDebugVector("W", W);

    /*
     * Calculate triangles
     */

    // C*A matrix
    GrB_Matrix CA;
    OK(GrB_Matrix_new(&CA, GrB_FP64, n, n))
    OK(GrB_mxm(CA, C, nullptr, GxB_PLUS_TIMES_FP64, C, A, nullptr))
    WriteOutDebugMatrix("CA", CA);

    // (C*A)*C matrix with element wise multiplication between A2*A
    GrB_Matrix CAC;
    OK(GrB_Matrix_new(&CAC, GrB_FP64, n, n))
    OK(GrB_eWiseMult_Matrix_Monoid(CAC, nullptr, nullptr, GxB_TIMES_FP64_MONOID, CA, C, nullptr))
    WriteOutDebugMatrix("CAC", CAC);

    // Determine triangles by (C*A)*C row sum
    GrB_Vector Tr;
    OK(GrB_Vector_new(&Tr, GrB_FP64, n))
    OK(GrB_Matrix_reduce_Monoid(Tr, nullptr, nullptr, GxB_PLUS_FP64_MONOID, CAC, nullptr))
    WriteOutDebugVector("Tr", Tr);

    /*
     * Calculate LCC
     */
    // Create an unary operator for calculating combinations
    GrB_Monoid lccDivMonoid;
    GrB_Monoid_new_FP64(&lccDivMonoid, GrB_DIV_FP64, 1.0);

    GrB_Vector LCC;
    OK(GrB_Vector_new(&LCC, GrB_FP64, n))
    OK(GrB_eWiseMult_Vector_Monoid(LCC, nullptr, nullptr, lccDivMonoid, Tr, W, nullptr))
    WriteOutDebugVector("LCC", LCC);

    printf("Processing ends at: %lu\n", time(nullptr));

    WriteOutResult(benchmarkParameters, mapping, LCC);

    GrB_UnaryOp_free(&combinationOp);
    GrB_Matrix_free(&A);
    GrB_Matrix_free(&At);
    GrB_Matrix_free(&CA);
    GrB_Matrix_free(&CAC);
    GrB_Vector_free(&Cr);
    GrB_Vector_free(&W);
    GrB_Vector_free(&Tr);
    GrB_Monoid_free(&lccDivMonoid);
    GrB_Vector_free(&LCC);

#ifdef PRINT_RESULT
    WriteOutDebugMatrix("A3", A3);
#endif

    GrB_finalize();
}

void lcc_undir(BenchmarkParameters benchmarkParameters) {
    GrB_init(GrB_NONBLOCKING);

    GrB_Matrix A;
    std::cout << "Loading" << std::endl;
    IndexMap mapping = ReadMatrix(benchmarkParameters, A);

    // Variable required by the OK macro
    unsigned int info;

    // The size of Adj
    unsigned long n;

    // Get the number of vertices, or size of the adjacency matrix.
    OK(GrB_Matrix_nrows(&n, A));
    WriteOutDebugMatrix("A", A);

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;

    /*
     * Calculate A derivatives
     */

    // Row sum of A
    GrB_Vector Ar;
    OK(GrB_Vector_new(&Ar, GrB_FP64, n))
    OK(GrB_Matrix_reduce_Monoid(Ar, nullptr, nullptr, GxB_PLUS_FP64_MONOID, A, nullptr))
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
    OK(GrB_Vector_apply(W, nullptr, nullptr, combinationOp, Ar, nullptr))
    WriteOutDebugVector("W", W);

    /*
     * Calculate triangles
     */

    // A^2 matrix
    GrB_Matrix A2;
    OK(GrB_Matrix_new(&A2, GrB_FP64, n, n))
    OK(GrB_mxm(A2, A, nullptr, GxB_PLUS_TIMES_FP64, A, A, nullptr))
    WriteOutDebugMatrix("A2", A2);

    // ~A^3 matrix with element wise multiplication between A2*A
    GrB_Matrix A2A;
    OK(GrB_Matrix_new(&A2A, GrB_FP64, n, n))
    OK(GrB_eWiseMult_Matrix_Monoid(A2A, nullptr, nullptr, GxB_TIMES_FP64_MONOID, A2, A, nullptr))
    WriteOutDebugMatrix("A2A", A2A);

    // Determine triangles by A2A row sum
    GrB_Vector Tr;
    OK(GrB_Vector_new(&Tr, GrB_FP64, n))
    OK(GrB_Matrix_reduce_Monoid(Tr, nullptr, nullptr, GxB_PLUS_FP64_MONOID, A2A, nullptr))
    WriteOutDebugVector("Tr", Tr);

    /*
     * Calculate LCC
     */
    // Create an unary operator for calculating combinations
    GrB_Monoid lccDivMonoid;
    GrB_Monoid_new_FP64(&lccDivMonoid, GrB_DIV_FP64, 1.0);

    GrB_Vector LCC;
    OK(GrB_Vector_new(&LCC, GrB_FP64, n))
    OK(GrB_eWiseMult_Vector_Monoid(LCC, nullptr, nullptr, lccDivMonoid, Tr, W, nullptr))
    WriteOutDebugVector("LCC", LCC);

    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    WriteOutResult(benchmarkParameters, mapping, LCC);

    GrB_UnaryOp_free(&combinationOp);
    GrB_Matrix_free(&A);
    GrB_Matrix_free(&A2);
    GrB_Matrix_free(&A2A);
    GrB_Vector_free(&Ar);
    GrB_Vector_free(&W);
    GrB_Vector_free(&Tr);
    GrB_Vector_free(&LCC);
    GrB_Monoid_free(&lccDivMonoid);

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