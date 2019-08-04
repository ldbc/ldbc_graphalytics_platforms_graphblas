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

int64_t GetMostFrequentElement(const int64_t *array, GrB_Index nvalues) {
    int64_t most_frequent_val = array[0];
    int64_t most_frequenc_occurence = 1;

    int64_t curr_value = array[0];
    int64_t curr_occurence = 1;

    for (GrB_Index i = 1; i < nvalues; i++) {
        int64_t value = array[i];

        if (curr_value == value) {
            curr_occurence++;
        } else if (curr_occurence >= most_frequenc_occurence) {
            most_frequent_val = curr_value;
            most_frequenc_occurence = curr_occurence;
            curr_value = value;
            curr_occurence = 1;
        }
    }
    if (curr_occurence >= most_frequenc_occurence) {
        most_frequent_val = curr_value;
    }

    return most_frequent_val;
}

void WriteOutCDLPResult(
    const GrB_Vector result,
    const std::vector<GrB_Index> &mapping,
    const BenchmarkParameters &parameters
) {
    std::ofstream file{parameters.output_file};
    if (!file.is_open()) {
        std::cerr << "File " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }

    bool value;
    for (GrB_Index res_index = 0; res_index < mapping.size(); res_index++) {
        GrB_Index original_index = mapping[res_index];
        GrB_Index matrix_index = res_index;

        GrB_Info info = GrB_Vector_extractElement_BOOL(&value, result, matrix_index);
        if (info == GrB_SUCCESS) {
            file << original_index << " " << value << std::endl;
        } else {
            file << original_index << " NaN" << std::endl;
        }
    }
}

GrB_Vector CDLP(GrB_Matrix A, bool directed) {
    ComputationTimer total_timer{"CommunityDetectionByLabelPropagation"};
    GrB_Info info;

    GrB_Index n;
    GrB_Matrix_nrows(&n, A);
    OK(GxB_Matrix_fprint(A, "A", GxB_COMPLETE, stdout))

    // Create the result vector
    GrB_Vector labels;
    OK(GrB_Vector_new(&labels, GrB_INT64, n))

    // The initial label matrix
    GrB_Matrix IL;
    OK(GrB_Matrix_new(&IL, GrB_INT64, n, n))
    for (GrB_Index i = 0; i < n; i++) {
        OK(GrB_Matrix_setElement_INT64(IL, i + 1, i, i))
    }
    OK(GxB_Matrix_fprint(IL, "IL", GxB_COMPLETE, stdout))

    // Matrix containing the labelled adjacency matrix
    GrB_Matrix L;
    OK(GrB_Matrix_new(&L, GrB_INT64, n, n))
    OK(GrB_mxm(L, A, GrB_NULL, GxB_PLUS_TIMES_INT64, A, IL, GrB_NULL))
    OK(GrB_Matrix_free(&IL))
    OK(GxB_Matrix_fprint(L, "L", GxB_COMPLETE, stdout))

    {
        ComputationTimer timer{"Row extract", total_timer};

        GrB_Descriptor row_desc;
        OK(GrB_Descriptor_new(&row_desc))
        OK(GrB_Descriptor_set(row_desc, GrB_INP0, GrB_TRAN))

        GrB_Vector row;
        OK(GrB_Vector_new(&row, GrB_INT64, n))
        for (GrB_Index i = 0; i < n; i++) {
            OK(GrB_Col_extract(row, GrB_NULL, GrB_NULL, L, GrB_ALL, n, i, row_desc))
            std::string row_name = "row_" + std::to_string(i);
            OK(GxB_Vector_fprint(row, row_name.c_str(), GxB_COMPLETE, stdout))

            GrB_Index row_vals;
            OK(GrB_Vector_nvals(&row_vals, row))
            if (row_vals == 0) {
                continue;
            }

            GrB_Index ntuple = row_vals;
            auto values = new int64_t[row_vals];

            OK(GrB_Vector_extractTuples_INT64(GrB_NULL, values, &ntuple, row))

            std::cout << row_name << " - " << ntuple << std::endl;
            for (GrB_Index j = 0; j < ntuple; j++) {
                std::cout << "\t" << std::to_string(values[j]) << std::endl;
            }
            std::sort(values, values + ntuple);
            int64_t most_frequent = GetMostFrequentElement(values, ntuple);
            std::cout << "Most frequent: " << std::to_string(most_frequent);

            delete[] values;
        }
    }

    GrB_Vector_free(&labels);
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_init();
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, parameters.thread_num);

    GrB_Matrix A = ReadMatrixMarket(parameters);
    std::vector<GrB_Index> mapping = ReadMapping(parameters);

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    GrB_Vector result = CDLP(A, parameters.directed);
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    //WriteOutCDLPResult(result, mapping, parameters);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
