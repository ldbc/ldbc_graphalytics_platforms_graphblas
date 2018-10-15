//
// Created by lehel on 10/15/18.
//

#ifndef GRAPHBLAS_BFS_SOURCE_H
#define GRAPHBLAS_BFS_SOURCE_H
#include "GraphBLAS.h"

GrB_Info bfs(GrB_Vector *v_output, GrB_Matrix A, GrB_Index startVertex, bool isWcc, int wccLevel);

#endif //GRAPHBLAS_BFS_SOURCE_H
