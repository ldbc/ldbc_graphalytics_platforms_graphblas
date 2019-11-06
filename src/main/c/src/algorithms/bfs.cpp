/*
 * BFS algorithm implementation in GraphBLAS.
 */

#include <algorithm>

extern "C" {
#include <GraphBLAS.h>
#include <LAGraph.h>
}

#include "utils.h"
#include "graphio.h"
#include "computation_timer.hpp"

void WriteOutBFSResult(
    GrB_Vector result,
    const std::vector<GrB_Index> &mapping,
    const BenchmarkParameters &parameters,
    int offset = 0
) {
    std::ofstream file{parameters.output_file};
    if (!file.is_open()) {
        std::cerr << "File " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }

    GrB_Index value;
    for (GrB_Index res_index = 0; res_index < mapping.size(); res_index++) {
        GrB_Index original_index = mapping[res_index];
        GrB_Index matrix_index = res_index;

        GrB_Info info = GrB_Vector_extractElement_UINT64(&value, result, matrix_index);
        if (info == GrB_SUCCESS) {
            file << original_index << " " << (value + offset) << std::endl;
        } else {
            file << original_index << " " << "9223372036854775807" << std::endl;
        }
    }
}

GrB_Vector LA_BFS(GrB_Matrix A, GrB_Index sourceVertex, bool directed) {
    ComputationTimer timer{"BFS"};

    GrB_Info info;

    GrB_Matrix AT = NULL;
    if (directed) {
        GrB_transpose(AT, NULL, NULL, A, NULL);
    } else {
        AT = A;
    }

    GrB_Vector v;
    OK(LAGraph_bfs_pushpull(&v, NULL, A, AT, sourceVertex, 0, false))

    return v;
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_init();
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, parameters.thread_num);

    GrB_Matrix A = ReadMatrixMarket(parameters);
    std::vector<GrB_Index> mapping = ReadMapping(parameters);

    auto source_vertex_iter = std::find(
            mapping.begin(),
            mapping.end(),
            parameters.source_vertex
    );
    if (source_vertex_iter == mapping.end()) {
        std::cout << "Source vertex not found in mapping" << std::endl;
        return -1;
    }
    GrB_Index sourceVertex = std::distance(mapping.begin(), source_vertex_iter);

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    GrB_Vector result = LA_BFS(A, sourceVertex, parameters.directed);
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    WriteOutBFSResult(result, mapping, parameters, -1);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
