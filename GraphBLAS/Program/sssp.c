//
// Created by lehel on 10/15/18.
//

#include "demos.h"
#include "printer.h"
#include <math.h>
#include <stdio.h>

double log2(double n) {
    return log(n)/log(2);
}

int main (int argc, char **argv) {

    GrB_Index n = 11;
    GrB_Matrix A = NULL;
    GrB_Vector v = NULL;

    GrB_Index* X = NULL;
    GrB_Index* Y = NULL;
    int* values = NULL;

    //X = malloc(n * sizeof (GrB_Index));
    //Y = malloc(n * sizeof (GrB_Index));
    X = malloc(n * sizeof(GrB_Index));
    Y = malloc(n * sizeof(GrB_Index));
    values = malloc(n * sizeof(int));

    FILE *myFile;
    myFile = fopen("/home/lehel/Downloads/SuiteSparse/GraphBLAS/Demo/Matrix/sssp", "r");

    unsigned int line = 0;

    uint32_t index1;
    uint32_t index2;
    int value;

    printf("start:\n");
    while(scanf("%u %u %d", &index1, &index2, &value) != EOF) {
        printf("line: %d\n", line);
        X[line] = index1;
        Y[line] = index2;
        values[line] = value;
        line = line + 1;
    }
    printf("end:\n");

    for (int i = 0; i < line; i++) {
        printf("%u %u %d\n", (uint32_t) X[i], (uint32_t) Y[i], values[i]);
    }

    GrB_Matrix_new(&A, GrB_INT32, 9, 9);
    unsigned int tuples = line + 1;


    GrB_Matrix_build(A, X, Y, values, 11, GrB_FIRST_INT32);

    int a;
    GrB_Matrix_extractElement(&a, A, 8, 7);

    printf("---- a --- : %d\n", a);

    printf("1\n");
    printMatrix(A);
    printf("2\n");


    // Read A froms stdin as bool.
    //read_matrix(&A, stdin, false, false, false, false, true);
    /*GrB_Matrix_nrows(&n, A);

    unsigned int element;
    GrB_Matrix_extractElement(&element, A, 5, 7);
    printf("element: %u \n", element);


    printMatrix(A);*/

}