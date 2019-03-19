#include "GraphBLAS.h"
#include "utils.h"

// Controls if debug information should be printed
#define VERBOSE

/*
 * ARGUMENT PARSING FUNCTIONS
 */

BenchmarkParameters ParseCommandLineParameters(int argc, char** argv) {
    BenchmarkParameters benchmarkParameters;

    for(int i = 0; i < argc; i++) {
        char* key = argv[i];
        char* value = argv[i+1];

        if (strcmp(key, "--directed") == 0) {
            if (strcmp(value, "true") == 0) {
                benchmarkParameters.directed = true;
            } else {
                benchmarkParameters.directed = false;
            }
        }
        if (strcmp(key, "--num-edges") == 0) {
            benchmarkParameters.numEdges = strtoul(value, NULL, 10);
        }
        if (strcmp(key, "--dataset") == 0) {
            benchmarkParameters.inputDir = value;
        }
        if (strcmp(key, "--output") == 0) {
            benchmarkParameters.outputFile = value;
        }
    }

    return benchmarkParameters;
}

/*
 * DEBUG FUNCTIONS
 */

void WriteOutDebugMatrix(const char *title, GrB_Matrix result) {
#ifdef VERBOSE
    printf("%s:\n", title);
    GrB_Index size;
    GrB_Matrix_nrows(&size, result);
    unsigned int element;

    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            GrB_Info info = GrB_Matrix_extractElement(&element, result, i, j);

            if (info == GrB_SUCCESS) {
                printf("%u ", element);
            } else if (info == GrB_NO_VALUE) {
                // It is up to the user to determine what 'no value'
                // means.  It depends on the semiring used.
                //printf(" [no value] ");
                printf("%u ", 0);
            } else {
                printf("Error! %s\n", GrB_error());
            }

        }
        printf("\n");
    }
#endif
}

void WriteOutDebugVector(const char *title, GrB_Vector result) {
#ifdef VERBOSE
    printf("%s:\n", title);
    GrB_Index size;
    GrB_Vector_size(&size, result);
    double element;

    for (unsigned int i = 0; i < size; i++) {
        GrB_Info info = GrB_Vector_extractElement(&element, result, i);

        if (info == GrB_SUCCESS) {
            printf("%lf ", element);
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
