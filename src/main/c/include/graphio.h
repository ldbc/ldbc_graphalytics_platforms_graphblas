#pragma once

#include "GraphBLAS.h"
#include "utils.h"

// Loading function
void ReadMatrix(BenchmarkParameters parameters, GrB_Matrix *A);

// Saving function
void WriteOutResult(BenchmarkParameters parameters, GrB_Vector existenceIndicator, GrB_Vector result);