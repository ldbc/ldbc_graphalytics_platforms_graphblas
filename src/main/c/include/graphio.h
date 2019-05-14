#pragma once

#include <unordered_map>

extern "C" {
#include "GraphBLAS.h"
}

#include "utils.h"

typedef std::unordered_map<GrB_Index, GrB_Index> IndexMap;

// Loading function
IndexMap ReadMatrix(BenchmarkParameters parameters, GrB_Matrix &A, bool weighted = false);
