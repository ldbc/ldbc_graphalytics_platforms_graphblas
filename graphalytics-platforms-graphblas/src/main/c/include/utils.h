#pragma once

#include <chrono>

extern "C" {
#include <GraphBLAS.h>
}

/*
 * ARGUMENT PARSING FUNCTIONS
 */
typedef struct {
    bool directed;
    unsigned long numVertices;
    char* inputDir;
    char* outputFile;
} BenchmarkParameters;

BenchmarkParameters ParseCommandLineParameters(int argc, char** argv);

time_t GetCurrentMilliseconds();

/*
 * DEBUG FUNCTIONS
 */

void WriteOutDebugVector(const char *title, GrB_Vector result);
void WriteOutDebugMatrix(const char *title, GrB_Matrix result);


/*
 * GRAPHBLAS HELPER MACROS
 */

//------------------------------------------------------------------------------
// OK: call a GraphBLAS method and check the result
//------------------------------------------------------------------------------

// OK(method) is a macro that calls a GraphBLAS method and checks the status;
// if a failure occurs, it handles the error via the CHECK macro above, and
// returns the error status to the caller.

#define OK(method)                                             \
{                                                              \
    info = method;                                             \
    if (info != GrB_SUCCESS)                                   \
    {                                                          \
        fprintf(stderr, "GraphBLAS error:\n%s\n", GrB_error());\
        exit(-100);                                            \
    }                                                          \
}
