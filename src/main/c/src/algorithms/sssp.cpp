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
        GrB_Vector_extractElement_FP64(&value, result, matrixIndex);

        if (value == INF_VALUE) {
            file << originalIndex << " infinity" << std::endl;
//            std::cout << originalIndex << " infinity" << std::endl;
        } else {
            file << originalIndex << " " << std::scientific << value << std::endl;
//            std::cout << originalIndex << " " << std::scientific << value << std::endl;
        }

    }
}

void SSSP(BenchmarkParameters benchmarkParameters) {
    GrB_init(GrB_NONBLOCKING);

    // Variable required by the OK macro
    unsigned int info;

    IndexMap mapping;
    GrB_Matrix A;
    {
        ComputationTimer timer{"Loading"};
        mapping = ReadMatrix(benchmarkParameters, A, true);
    }

    std::cout << "====" << std::endl;
    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    std::cout << "====" << std::endl << std::endl;

    GrB_Index n;
    GrB_Index nvals;
    {
        ComputationTimer timer{"A"};
        OK(GrB_Matrix_nvals(&nvals, A))
        OK(GrB_Matrix_nrows(&n, A))
    }

    GrB_Index mappedSourceVertex = mapping[benchmarkParameters.sourceVertex];
    GrB_Vector dist;
    {
        ComputationTimer timer{"dist"};

        // Mapping indexes are increased by +1, so the size should be
        // always bigger than the indices inside
        GrB_Index inf_value = mapping.size();
        OK(GrB_Vector_new(&dist, GrB_FP64, n))
        OK(GrB_Vector_assign_FP64(dist, nullptr, nullptr, inf_value, GrB_ALL, n, nullptr))
        OK(GrB_Vector_setElement_FP64(dist, 0, mappedSourceVertex))
    }

    {
        ComputationTimer timer{"SSSP"};

        GrB_Descriptor desc;
        OK(GrB_Descriptor_new(&desc))
        OK(GrB_Descriptor_set(desc, GrB_OUTP, GrB_REPLACE))

        for (GrB_Index level = 0; level < n; level++) {
            std::string levelName = "level-" + std::to_string(level);
            ComputationTimer levelTimer{levelName.c_str()};

            // TODO: Masking
            OK(GrB_vxm(
                dist,
                nullptr,
                GrB_MIN_FP64,
                GxB_MIN_PLUS_FP64,
                dist,
                A, nullptr
            ))
        }
    }

    std::cout << "====" << std::endl;
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;
    std::cout << "====" << std::endl << std::endl;

    {
        ComputationTimer timer{"Serialization"};
        WriteOutSSSPResult(benchmarkParameters, mapping, dist);
    }

    GrB_Matrix_free(&A);
    GrB_Vector_free(&dist);

    GrB_finalize();
}

int main(int argc, char **argv) {
    BenchmarkParameters benchmarkParameters = ParseCommandLineParameters(argc, argv);
    SSSP(benchmarkParameters);
}