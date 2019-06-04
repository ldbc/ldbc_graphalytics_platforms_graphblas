#include <string>
#include <chrono>

#include "utils.h"

//#define VERBOSE

time_t GetCurrentMilliseconds() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

/*
 * ARGUMENT PARSING FUNCTIONS
 */

BenchmarkParameters ParseBenchmarkParameters(int argc, char **argv) {
    BenchmarkParameters benchmark_parameters;

    for (int i = 0; i < argc; i++) {
        char *key = argv[i];
        char *value = argv[i + 1];

        if (strcmp(key, "--source-vertex") == 0) {
            benchmark_parameters.source_vertex = std::stoul(value);
        }
        if (strcmp(key, "--directed") == 0) {
            benchmark_parameters.directed = (strcmp(value, "true") == 0);
        }
        if (strcmp(key, "--dataset") == 0) {
            benchmark_parameters.input_dir = value;
        }
        if (strcmp(key, "--output") == 0) {
            benchmark_parameters.output_file = value;
        }
        if (strcmp(key, "--threadnum") == 0) {
            benchmark_parameters.thread_num = std::stoul(value);
        }
    }

    return benchmark_parameters;
}

ConverterParameters ParseConverterParameters(int argc, char **argv) {
    ConverterParameters converter_parameters;

    for (int i = 0; i < argc; i++) {
        char *key = argv[i];
        char *value = argv[i + 1];

        if (strcmp(key, "--input-vertex") == 0) {
            converter_parameters.vertex_file = value;
        }
        if (strcmp(key, "--input-edge") == 0) {
            converter_parameters.edge_file = value;
        }
        if (strcmp(key, "--output-mm") == 0) {
            converter_parameters.market_file = value;
        }
        if (strcmp(key, "--output-vtx") == 0) {
            converter_parameters.mapping_file = value;
        }
        if (strcmp(key, "--weighted") == 0) {
            converter_parameters.weighted = (strcmp(value, "true") == 0);
        }
        if (strcmp(key, "--directed") == 0) {
            converter_parameters.directed = (strcmp(value, "true") == 0);
        }
    }

    return converter_parameters;
}

/*
 * DEBUG FUNCTIONS
 */

void WriteOutDebugMatrix(const char *title, GrB_Matrix result) {
    printf("%s:\n", title);
    GrB_Index size;
    GrB_Matrix_nrows(&size, result);
    double element;

    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            GrB_Info info = GrB_Matrix_extractElement_FP64(&element, result, i, j);

            if (info == GrB_SUCCESS) {
                printf("%g ", element);
            } else if (info == GrB_NO_VALUE) {
                // It is up to the user to determine what 'no value'
                // means.  It depends on the semiring used.
//                printf(" [no value] ");
                printf("- ");
            } else {
                printf("Error! %s\n", GrB_error());
            }

        }
        printf("\n");
    }
}

void WriteOutDebugVector(const char *title, GrB_Vector result) {
    printf("%s:\n", title);
#ifdef VERBOSE
    GrB_Index size;
    GrB_Vector_size(&size, result);
    double element;

    for (unsigned int i = 0; i < size; i++) {
        GrB_Info info = GrB_Vector_extractElement_FP64(&element, result, i);

        if (info == GrB_SUCCESS) {
            if (element == UINT64_MAX) {
                printf("inf ");
            } else {
                printf("%g ", element);
            }

//            printf("%u ", element);
        } else if (info == GrB_NO_VALUE) {
            // It is up to the user to determine what 'no value'
            // means.  It depends on the semiring used.
            printf(" [no value] ");
//            printf("%u ", 0);
        } else {
            printf("Error! %s\n", GrB_error());
        }

    }
    printf("\n");
#endif
}
