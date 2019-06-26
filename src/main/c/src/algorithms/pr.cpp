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

void WriteOutPRResult(
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

GrB_Vector PageRank(
    GrB_Matrix A,
    double dampening_factor,
    unsigned long iteration_num
) {
    ComputationTimer total_timer{"PageRank"};
    GrB_Info info;

    GrB_Index n;
    {
        ComputationTimer timer{"A nrows", total_timer};
        OK(GrB_Matrix_nrows(&n, A))
    }
    GrB_Index nvals;
    {
        ComputationTimer timer{"A nvals", total_timer};
        OK(GrB_Matrix_nvals(&nvals, A))
    }

//    GrB_Matrix A_t;
//    {
//        ComputationTimer timer{"A^T", total_timer};
//        OK(GrB_Matrix_new(&A_t, GrB_BOOL, n, n))
//        OK(GrB_transpose(
//            A_t,
//            GrB_NULL,
//            GrB_NULL,
//            A,
//            GrB_NULL
//        ))
//    }

    // Make a complement descriptor
    GrB_Descriptor invmask_desc;
    GrB_Descriptor_new(&invmask_desc);
    GrB_Descriptor_set(invmask_desc, GrB_MASK, GrB_SCMP);

    // Make a transpose descriptor
    GrB_Descriptor transpose_desc;
    GrB_Descriptor_new(&transpose_desc);
    GrB_Descriptor_set(transpose_desc, GrB_INP0, GrB_TRAN);
    GrB_Descriptor_set(transpose_desc, GrB_OUTP, GrB_REPLACE);

    //
    // Matrix A row sum
    //
    // Stores the outbound degrees of all vertices
    //
    GrB_Vector A_out;
    {
        ComputationTimer timer{"A row sum", total_timer};
        OK(GrB_Vector_new(&A_out, GrB_UINT64, n))
        OK(GrB_Matrix_reduce_Monoid(
            A_out,
            GrB_NULL,
            GrB_NULL,
            GxB_PLUS_UINT64_MONOID,
            A,
            GrB_NULL
        ))
    }

    //
    // Non-dangling_vec vector determination
    //
    // These vertices the ones which have outgoing edges. In further operations,
    // this mask can be negated to select dangling_vec vertices.
    //
    GrB_Vector nondangling_mask;
    {
        ComputationTimer timer{"nondangling mask", total_timer};
        OK(GrB_Vector_new(&nondangling_mask, GrB_BOOL, n))
        OK(GrB_Matrix_reduce_Monoid(
            nondangling_mask,
            GrB_NULL,
            GrB_NULL,
            GxB_LOR_BOOL_MONOID,
            A,
            GrB_NULL
        ))
    }

    //
    // Iteration
    //

    // Result vectors
    GrB_Vector *pr_prev = (GrB_Vector *) malloc(sizeof(GrB_Vector));
    GrB_Vector *pr_next = (GrB_Vector *) malloc(sizeof(GrB_Vector));
    GrB_Vector_new(pr_prev, GrB_FP64, n);
    GrB_Vector_new(pr_next, GrB_FP64, n);

    // Fill result vector with initial value (1 / |V|)
    {
        ComputationTimer timer{"Initial fill", total_timer};
        OK(GrB_Vector_assign_FP64(
            *pr_prev,
            GrB_NULL,
            GrB_NULL,
            (1.0 / n),
            GrB_ALL,
            n,
            GrB_NULL
        ))
    }

    GrB_Vector importance_vec;
    OK(GrB_Vector_new(&importance_vec, GrB_FP64, n))
    GrB_Vector dangling_vec;
    OK(GrB_Vector_new(&dangling_vec, GrB_FP64, n))

    // Teleporting factor
    const double teleport_factor = (1 - dampening_factor) / n;

    for (int i = 0; i < iteration_num; i++) {
        ComputationTimer iter_timer{"Iteration " + std::to_string(i), total_timer};

        //
        // Importance calculation
        //

        // Divide previous PageRank with number of outbound edges
        {
            ComputationTimer timer{"importance_vec = pr_prev / |V|", iter_timer};
            OK(GrB_eWiseMult_Vector_BinaryOp(
                importance_vec,
                nondangling_mask,
                GrB_NULL,
                GrB_DIV_FP64,
                *pr_prev,
                A_out,
                GrB_NULL
            ))
        }


        // Multiply importance with dampening factor
        {
            ComputationTimer timer{"importance_vec *= dampening", iter_timer};
            OK(GrB_Vector_assign_FP64(
                importance_vec,
                nondangling_mask,
                GrB_TIMES_FP64,
                dampening_factor,
                GrB_ALL,
                n,
                GrB_NULL
            ))
        }

        // Calculate summed PR for all inbound vertices
        {
            ComputationTimer timer{"importance_vec *= A^t", iter_timer};
            OK(GrB_mxv(
                importance_vec,
                nondangling_mask,
                GrB_NULL,
                GxB_PLUS_TIMES_FP64,
                A,
                importance_vec,
                transpose_desc
            ))
        }

        //
        // Dangling calculation
        //

        // Extract all the dangling PR entries from the previous result
        {
            ComputationTimer timer{"dangling extract", iter_timer};
            OK(GrB_Vector_extract(
                dangling_vec,
                nondangling_mask,
                GrB_NULL,
                *pr_prev,
                GrB_ALL,
                n,
                invmask_desc
            ))
        }

        // Sum the previous PR values together
        double dangling_factor;
        {
            ComputationTimer timer{"dangling sum", iter_timer};
            OK(GrB_Vector_reduce_FP64(
                &dangling_factor,
                GrB_NULL,
                GxB_PLUS_FP64_MONOID,
                dangling_vec,
                GrB_NULL
            ))
            dangling_factor *= (dampening_factor / n);
        }

        //
        // PageRank summarization
        // Add teleportation, importance_vec, and dangling_vec components together
        //
        {
            ComputationTimer pr_sum_timer{"PR sum", iter_timer};

            {
                ComputationTimer timer{
                    "pr_next = teleport + dangling",
                    pr_sum_timer
                };
                OK(GrB_Vector_assign_FP64(
                    *pr_next,
                    GrB_NULL,
                    GrB_NULL,
                    (teleport_factor + dangling_factor),
                    GrB_ALL,
                    n,
                    GrB_NULL
                ))
            }
            {
                ComputationTimer timer{
                    "pr_next += importance",
                    pr_sum_timer
                };
                OK(GrB_eWiseAdd_Vector_Monoid(
                    *pr_next,
                    GrB_NULL,
                    GrB_NULL,
                    GxB_PLUS_FP64_MONOID,
                    *pr_next,
                    importance_vec,
                    GrB_NULL
                ))
            }
        }

        // Swap the new and old
        std::swap(pr_prev, pr_next);
    }

    GrB_Descriptor_free(&transpose_desc);
    GrB_Descriptor_free(&invmask_desc);
    GrB_Matrix_free(&A);
    GrB_Vector_free(&A_out);
    GrB_Vector_free(&nondangling_mask);
    GrB_Vector_free(&importance_vec);
    GrB_Vector_free(&dangling_vec);
    GrB_Vector_free(pr_next);

    // Because of the swap, the previous result will be the actual output
    return *pr_prev;
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_init();
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, parameters.thread_num);

    GrB_Matrix A = ReadMatrixMarket(parameters);
    std::vector<GrB_Index> mapping = ReadMapping(parameters);

    std::cout << "Damping factor: " << parameters.damping_factor << std::endl;
    std::cout << "Max iteration: " << parameters.max_iteration << std::endl;

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    GrB_Vector result = PageRank(
        A,
        parameters.damping_factor,
        parameters.max_iteration
    );
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    WriteOutPRResult(result, mapping, parameters);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
