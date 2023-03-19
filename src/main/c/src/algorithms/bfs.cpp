/*
 * BFS algorithm implementation in GraphBLAS.
 */

#include <algorithm>

#include "utils.h"
#include "graphio.h"
#include "computation_timer.hpp"

void SerializeBFSResult(
    GrB_Vector result,
    const std::vector<GrB_Index> &mapping,
    const BenchmarkParameters &parameters
) {
    std::ofstream file{parameters.output_file};
    if (!file.is_open()) {
        std::cerr << "Output file " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }

    GrB_Info info;
    GrB_Index n = mapping.size();

    GrB_Index nvals;
    OK(GrB_Vector_nvals(&nvals, result));

    GrB_Index *I = NULL;
    I = (GrB_Index*) malloc ((nvals + 1) * sizeof (GrB_Index));

    int64_t *X = NULL;
    X = (int64_t *) malloc((nvals + 1) * sizeof(int64_t));

    // LAGraph returns uses int32 values if the number of vertices is sufficiently
    // small and int64s otherwise
    if (nvals > INT32_MAX) {
        OK(GrB_Vector_extractTuples_INT64(I, X, &nvals, result));
    } else {
        int32_t *Y = NULL;
        Y = (int32_t *) malloc((nvals + 1) * sizeof(int32_t));
        OK(GrB_Vector_extractTuples_INT32(I, Y, &nvals, result));
        for (int i = 0; i < nvals; i++) {
            X[i] = Y[i];
        }
    }

    // LAGraph returns:
    // * no element when a vertex is unreachable -> this should be represented as infinity in Graphalytics
    // * 0 for the vertex itself
    // * 1+ for vertices that are reachable (traversal level)
    GrB_Index curr_nz = 0;
    for (GrB_Index v_id = 0; v_id < n; v_id++) {
        GrB_Index original_index = mapping[v_id];
        int64_t level;

        if (I[curr_nz] == v_id) {
            level = X[curr_nz];
            curr_nz++;
        } else {
            level = 9223372036854775807;
        }

        file << original_index << " " << level << std::endl;
    }

    free(I);
    free(X);
}

GrB_Vector LA_BFS(GrB_Matrix A, GrB_Index sourceVertex, bool directed) {
    ComputationTimer timer{"BFS"};

    GrB_Info info;

    GrB_Vector v = NULL;
    LAGraph_Kind kind = directed ? LAGraph_ADJACENCY_DIRECTED : LAGraph_ADJACENCY_UNDIRECTED;
    LAGraph_Graph G;
    char msg[LAGRAPH_MSG_LEN];
    LAGraph_New(&G, &A, kind, msg);
    LAGr_BreadthFirstSearch(&v, NULL, G, sourceVertex, msg);

    return v;
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_Init(NULL);
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

    SerializeBFSResult(result, mapping, parameters);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
