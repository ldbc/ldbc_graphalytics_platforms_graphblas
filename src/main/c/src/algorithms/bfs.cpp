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

void SerializeBFSResult(
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

    GrB_Info info;
    GrB_Index n = mapping.size();

    GrB_Index nvals;
    OK(GrB_Vector_nvals(&nvals, result));

    GrB_Index *I = NULL;
    I = (GrB_Index*) LAGraph_malloc ((nvals + 1), sizeof (GrB_Index));

    int64_t *X = NULL;
    X = (int64_t *) LAGraph_malloc((nvals + 1), sizeof(int64_t));

    // LAGraph returns uses int32 values if the number of vertices is sufficiently
    // small and int64s otherwise
    if (nvals > INT32_MAX) {
        OK(GrB_Vector_extractTuples_INT64(I, X, &nvals, result));
    } else {
        int32_t *Y = NULL;
        Y = (int32_t *) LAGraph_malloc((nvals + 1), sizeof(int32_t));
        OK(GrB_Vector_extractTuples_INT32(I, Y, &nvals, result));
        for (int i = 0; i < nvals; i++) {
            X[i] = Y[i];
        }
    }

    GrB_Index curr_nz = 0;
    for (GrB_Index matrix_index = 0; matrix_index < n; matrix_index++) {
        GrB_Index original_index = mapping[matrix_index];

        if (I[curr_nz] == matrix_index) {
            int64_t value = X[curr_nz];
            file << original_index << " " << (value + offset) << std::endl;
            curr_nz++;
        } else {
            file << original_index << " " << "9223372036854775807" << std::endl;
        }
    }

    LAGraph_free(I);
    LAGraph_free(X);
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

    SerializeBFSResult(result, mapping, parameters, -1);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
