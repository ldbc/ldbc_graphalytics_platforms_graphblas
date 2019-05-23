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
#include "graphio.h"
#include "computation_timer.hpp"

/*
 * Result serializer function
 */
void WriteOutSSSPResult(BenchmarkParameters parameters, const IndexMap &mapping, GrB_Vector result) {
    const GrB_Index INF_VALUE = mapping.size();

    GrB_Info info;
    GrB_Index n;

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
        GrB_Info info = GrB_Vector_extractElement_FP64(&value, result, matrixIndex);

        if (info == GrB_SUCCESS) {
            file << originalIndex << " " << std::scientific << value << std::endl;
//            std::cout << originalIndex << " infinity" << std::endl;
        } else {
            file << originalIndex << " infinity" << std::endl;
//            std::cout << originalIndex << " " << std::scientific << value << std::endl;
        }

    }
}

int main(int argc, char **argv) {
    //GrB_init(GrB_NONBLOCKING);
    LAGraph_init();

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
    GrB_Index mappedSourceVertex = mapping[benchmarkParameters.sourceVertex];
    GrB_Vector d = nullptr;
    {
        ComputationTimer timer{"SSSP"};
        OK(LAGraph_BF_basic(&d, A, mappedSourceVertex))
    }
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    WriteOutSSSPResult(benchmarkParameters, mapping, d);
}