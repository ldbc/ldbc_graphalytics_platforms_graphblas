/*
 * LCC algorithm implementation in GraphBLAS.
 */

#include <iostream>
#include <ctime>
#include <fstream>

extern "C" {
#include <GraphBLAS.h>
}

#include "utils.h"
#include "graphio.h"
#include "computation_timer.hpp"

/*
 * Result serializer function
 */
void WriteOutLCCResult(
    const BenchmarkParameters parameters,
    const IndexMap &mapping,
    GrB_Vector result) {

    std::ofstream file{parameters.outputFile};
    if (!file.is_open()) {
        std::cerr << "File " << parameters.outputFile << " does not exists" << std::endl;
        exit(-1);
    }

    double value;
    for (auto mappedIndex : mapping) {
        GrB_Index originalIndex = mappedIndex.first;
        GrB_Index matrixIndex = mappedIndex.second;

        value = 0.0;
        GrB_Vector_extractElement_FP64(&value, result, matrixIndex);

        file << originalIndex << " " << std::scientific << value << std::endl;
        std::cout << originalIndex << " " << std::scientific << value << std::endl;
    }
}

/**
 * // Z = f(x) * f(x) - 1
 * This operator calculates the comb(d(v), 2) value.
 * Note: the division by two can be skipped, because it will be the final division in the LCC calculation
 * @param z
 * @param x
 */
void CalculateCombinations(void *z, const void *x) {
    double xd = *(double *) x;
    auto *zd = (double *) z;
    (*zd) = ((xd) * (xd - 1));
}

void Lcc(BenchmarkParameters benchmarkParameters) {
    GrB_init(GrB_NONBLOCKING);

    // Variable required by the OK macro
    unsigned int info;

    IndexMap mapping;
    GrB_Matrix A;
    {
        ComputationTimer timer{"Loading"};
        mapping = ReadMatrix(benchmarkParameters, A);
    }

    std::cout << "====" << std::endl;
    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    std::cout << "====" << std::endl << std::endl;

    // The size of Adj
    GrB_Index nvals;
    GrB_Index n;
    {
        ComputationTimer timer{"A finalization"};
        OK(GrB_Matrix_nvals(&nvals, A));
        OK(GrB_Matrix_nrows(&n, A))
        WriteOutDebugMatrix("A", A);
    }

    GrB_Matrix C;

    /*
     * Create undirected C matrix,
     * by computing A (+) At using a boolean binary operator
     */
    {
        GrB_Descriptor desc;
        OK (GrB_Descriptor_new(&desc))
        OK (GrB_Descriptor_set(desc, GrB_INP1, GrB_TRAN))

        ComputationTimer timer{"C"};

        OK(GrB_Matrix_new(&C, GrB_FP64, n, n))
        OK(GrB_eWiseAdd_Matrix_BinaryOp(C, nullptr, nullptr, GxB_LOR_BOOL, A, A, desc))
        WriteOutDebugMatrix("C", C);
    }

    /*
     * Calculate A derivatives
     */

    // Row sum of A
    GrB_Vector Cr;
    {
        ComputationTimer timer{"Cr"};

        OK(GrB_Vector_new(&Cr, GrB_FP64, n))
        OK(GrB_Matrix_reduce_Monoid(Cr, nullptr, nullptr, GxB_PLUS_FP64_MONOID, C, nullptr))
        WriteOutDebugVector("Cr", Cr);
    }

    /*
     * Calculate wedges
     */

    // Create an unary operator for calculating combinations
    GrB_UnaryOp combinationOp;
    GrB_UnaryOp_new(&combinationOp, &CalculateCombinations, GrB_FP64, GrB_FP64);

    // Create vector W for containing number of wedges per vertex
    // Wedges per vertex is calculated by (d(v) * (d(v) - 1)
    GrB_Vector W;
    {
        ComputationTimer timer{"W"};

        OK(GrB_Vector_new(&W, GrB_FP64, n))
        OK(GrB_Vector_apply(W, nullptr, nullptr, combinationOp, Cr, nullptr))
        WriteOutDebugVector("W", W);
    }

    /*
     * Calculate triangles
     */

    // CA<C> = C*At' using a masked dot product
    GrB_Matrix CA;
    {
        GrB_Descriptor desc;
        OK (GrB_Descriptor_new(&desc))
        OK (GrB_Descriptor_set(desc, GrB_INP1, GrB_TRAN))

        ComputationTimer timer{"CA"};

        OK(GrB_Matrix_new(&CA, GrB_FP64, n, n))
        OK(GrB_mxm(CA, C, nullptr, GxB_PLUS_TIMES_FP64, C, A, desc))
        WriteOutDebugMatrix("CA", CA);
    }

    // (C*A)*C matrix with element wise multiplication between A2*A
    GrB_Matrix CAC;
    {
        ComputationTimer timer{"CAC"};

        OK(GrB_Matrix_new(&CAC, GrB_FP64, n, n))
        OK(GrB_eWiseMult_Matrix_Monoid(CAC, nullptr, nullptr, GxB_TIMES_FP64_MONOID, CA, C, nullptr))
        WriteOutDebugMatrix("CAC", CAC);
    }

    // Determine triangles by (C*A)*C row sum
    GrB_Vector Tr;
    {
        ComputationTimer timer{"Tr"};

        OK(GrB_Vector_new(&Tr, GrB_FP64, n))
        OK(GrB_Matrix_reduce_Monoid(Tr, nullptr, nullptr, GxB_PLUS_FP64_MONOID, CAC, nullptr))
        WriteOutDebugVector("Tr", Tr);
    }


    /*
     * Calculate LCC
     */
    // Create an unary operator for calculating combinations
    GrB_Monoid lccDivMonoid;
    GrB_Monoid_new_FP64(&lccDivMonoid, GrB_DIV_FP64, 1.0);

    GrB_Vector LCC;
    {
        ComputationTimer timer{"LCC"};

        OK(GrB_Vector_new(&LCC, GrB_FP64, n))
        OK(GrB_eWiseMult_Vector_Monoid(LCC, nullptr, nullptr, lccDivMonoid, Tr, W, nullptr))
        WriteOutDebugVector("LCC", LCC);
    }

    std::cout << "====" << std::endl;
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;
    std::cout << "====" << std::endl << std::endl;

    {
        ComputationTimer timer{"Serialization"};
        WriteOutLCCResult(benchmarkParameters, mapping, LCC);
    }

    GrB_UnaryOp_free(&combinationOp);
    GrB_Matrix_free(&A);
    GrB_Matrix_free(&CA);
    GrB_Matrix_free(&CAC);
    GrB_Vector_free(&Cr);
    GrB_Vector_free(&W);
    GrB_Vector_free(&Tr);
    GrB_Monoid_free(&lccDivMonoid);
    GrB_Vector_free(&LCC);

    GrB_finalize();
}

int main(int argc, char **argv) {
    BenchmarkParameters benchmarkParameters = ParseCommandLineParameters(argc, argv);
    Lcc(benchmarkParameters);
}
