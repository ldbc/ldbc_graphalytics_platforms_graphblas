/*
 * LCC algorithm implementation in GraphBLAS.
 */

#include <iostream>
#include <ctime>
#include <fstream>

extern "C" {
#include <GraphBLAS.h>
#include <LAGraph.h>
}

#include "utils.h"
#include "computation_timer.hpp"

int main(int argc, char **argv) {
    //GrB_init(GrB_NONBLOCKING);
    LAGraph_init();
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, 1);

    GrB_Info info;

    BenchmarkParameters benchmarkParameters = ParseCommandLineParameters(argc, argv);

    IndexMap mapping;
    GrB_Matrix A;
    {
        ComputationTimer timer{"Loading"};
        mapping = ReadMatrix(benchmarkParameters, A, true);
    }
    {
        ComputationTimer timer{"Diagonal"};
        for (GrB_Index i = 0; i < 3; i++) {
            OK(GrB_Matrix_setElement_FP64(A, 0.0, i, i))
        }
    }

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    GrB_Index mappedSourceVertex = mapping[benchmarkParameters.source_vertex];
    GrB_Vector d = nullptr;
    {
        ComputationTimer timer{"SSSP"};
        OK(LAGraph_BF_basic(&d, A, mappedSourceVertex))
    }
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    WriteOutSSSPResult(benchmarkParameters, mapping, d);
}
