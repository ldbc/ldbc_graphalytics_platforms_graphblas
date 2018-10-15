//
// Created by lehel on 10/15/18.
//

#include "printer.h"

void printVector(GrB_Vector vec) {
    GrB_Index size;
    GrB_Vector_size(&size, vec);
    printf("Size: %d\n", (int)size);
    unsigned int element;
    for(unsigned int i = 0; i < size; i++) {
        GrB_Vector_extractElement(&element, vec, i);
        printf("%u ", element);
    }
    printf("\n");
}

void printMatrix(GrB_Matrix M) {
    GrB_Index size;
    GrB_Matrix_nrows(&size, M);
    unsigned int element;
    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            GrB_Matrix_extractElement(&element, M, i, j);
            printf("%u ", element);
        }
        printf("\n");
    }
    printf("\n");
}