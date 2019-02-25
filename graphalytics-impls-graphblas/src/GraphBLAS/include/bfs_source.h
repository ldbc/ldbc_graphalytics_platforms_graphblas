//
// Created by lehel on 10/15/18.
//

#pragma once

#include "GraphBLAS.h"

GrB_Info bfs(GrB_Vector *v_output, GrB_Matrix A, GrB_Index startVertex, bool isWcc, int wccLevel);