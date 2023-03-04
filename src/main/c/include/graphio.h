#pragma once

#include <vector>
#include <fstream>
#include <iostream>

#include "utils.h"

GrB_Matrix ReadMatrixMarket(const BenchmarkParameters& parameters);

std::vector<GrB_Index> ReadMapping(const BenchmarkParameters& parameters);
