/*
 * LCC algorithm implementation in GraphBLAS.
 */

#include <algorithm>

extern "C" {
#include <GraphBLAS.h>
#include <LAGraph.h>
}

#include "utils.h"
#include "graphio.h"
#include "computation_timer.hpp"

void WriteOutWCCResult(
    const GrB_Vector result,
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

    double value;
    for (GrB_Index res_index = 0; res_index < mapping.size(); res_index++) {
        GrB_Index original_index = mapping[res_index];
        GrB_Index matrix_index = res_index;

        GrB_Info info = GrB_Vector_extractElement_FP64(&value, result, matrix_index);
        if (info == GrB_SUCCESS) {
            file << original_index << " " << value << std::endl;
        } else {
            file << original_index << " infinity" << std::endl;
        }
    }
}

GrB_Vector WeaklyConnectedComponents(GrB_Matrix A, bool directed) {
    ComputationTimer total_timer{"WeaklyConnectedComponents"};
    GrB_Info info;

    GrB_Index n;
    GrB_Matrix_nrows(&n, A);

    GrB_Matrix C = A;
    {
        ComputationTimer timer{"Zero diagonalize"};
        for (GrB_Index i = 0; i < n; i++) {
            OK(GrB_Matrix_setElement_FP64(C, 0, i, i))
        }
    }

    if (directed) {
        OK(GrB_Matrix_new(&C, GrB_BOOL, n, n))

        GrB_Descriptor desc;
        OK(GrB_Descriptor_new(&desc))
        OK(GrB_Descriptor_set(desc, GrB_INP1, GrB_TRAN))

        OK(GrB_eWiseAdd_Matrix_BinaryOp(C, GrB_NULL, GrB_NULL, GrB_LOR, A, A, desc))
        OK(GrB_Descriptor_free(&desc))
    } else {
        C = A;
    }

    WriteOutDebugMatrix("C", C);

    GrB_Vector parents = LAGraph_lacc(C);
    GxB_Vector_fprint(parents, "parents", GxB_COMPLETE, stderr);
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_init();
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, parameters.thread_num);

    GrB_Matrix A = ReadMatrixMarket(parameters);
    std::vector<GrB_Index> mapping = ReadMapping(parameters);

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    WeaklyConnectedComponents(A, parameters.directed);
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    //WriteOutPRResult(result, mapping, parameters);

    GrB_Matrix_free(&A);
    //GrB_Vector_free(&result);
}
