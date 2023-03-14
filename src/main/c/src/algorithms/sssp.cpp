/*
 * SSSP algorithm implementation in GraphBLAS.
 */

#include <algorithm>

#include "utils.h"
#include "graphio.h"
#include "computation_timer.hpp"

void SerializeSSSPResult(
    GrB_Vector result,
    const std::vector<GrB_Index> &mapping,
    const BenchmarkParameters &parameters
) {
    std::ofstream file{parameters.output_file};
    if (!file.is_open()) {
        std::cerr << "File " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }
    file.precision(16);
    file << std::scientific;

    GrB_Info info;
    GrB_Index n = mapping.size();

    GrB_Index nvals;
    OK(GrB_Vector_nvals(&nvals, result));

    GrB_Index *I = NULL;
    I = (GrB_Index*) malloc ((nvals + 1) * sizeof (GrB_Index));

    double *X = NULL;
    X = (double *) malloc((nvals + 1) * sizeof(double));
    OK(GrB_Vector_extractTuples_FP64(I, X, &nvals, result));

    GrB_Index curr_nz = 0;
    for (GrB_Index matrix_index = 0; matrix_index < n; matrix_index++) {
        GrB_Index original_index = mapping[matrix_index];

        if (I[curr_nz] == matrix_index) {
            file << original_index << " " << X[curr_nz] << std::endl;
            curr_nz++;
        } else {
            file << original_index << " infinity" << std::endl;
        }
    }

    free(I);
    free(X);
}

GrB_Vector LA_SSSP(GrB_Matrix A, GrB_Index sourceVertex, bool directed) {
    GrB_Info info;

    // The size of Adj
    GrB_Index n;
    OK(GrB_Matrix_nrows(&n, A))

    for (GrB_Index i = 0; i < n; i++) {
        OK(GrB_Matrix_setElement_FP64(A, 0.0, i, i))
    }

    ComputationTimer timer{"SSSP"};

    LAGraph_Kind kind = directed ? LAGraph_ADJACENCY_DIRECTED : LAGraph_ADJACENCY_UNDIRECTED;
    LAGraph_Graph G;

    GrB_Vector d = NULL;
    double delta = 2.5;

    GrB_Scalar Delta = NULL ;
    GrB_Scalar_new(&Delta, GrB_FP64);
    GrB_Scalar_setElement_FP64(Delta, delta);
    char msg[LAGRAPH_MSG_LEN];
    LAGraph_New(&G, &A, kind, msg);
    LAGraph_Cached_EMin(G, msg);
    LAGr_SingleSourceShortestPath(&d, G, sourceVertex, Delta, msg);

    return d;
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
    GrB_Vector result = LA_SSSP(A, sourceVertex, parameters.directed);
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    SerializeSSSPResult(result, mapping, parameters);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
