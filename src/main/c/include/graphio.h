#pragma once

#include <map>

extern "C" {
#include "GraphBLAS.h"
}

#include "utils.h"

typedef std::map<GrB_Index, GrB_Index> IndexMap;

// Loading function
IndexMap ReadMatrix(BenchmarkParameters parameters, GrB_Matrix& A);