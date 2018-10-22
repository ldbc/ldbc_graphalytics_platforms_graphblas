//
// Created by lehel on 10/15/18.
//

#include "printer.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

void printIntVector(GrB_Vector vec) {
    GrB_Index size;
    GrB_Vector_size(&size, vec);
    unsigned int element;
    for(unsigned int i = 0; i < size; i++) {
        GrB_Info info = GrB_Vector_extractElement(&element, vec, i);

        if (info == GrB_SUCCESS)
        {
            //printf("%lf ", element);
            printf(RED "%u " RESET, element);
        }
        else if (info == GrB_NO_VALUE)
        {
            // It is up to the user to determine what 'no value'
            // means.  It depends on the semiring used.
            //printf(" [no value] ");
            printf("%u ", 0);
        }
        else
        {
            printf ("Error! %s\n", GrB_error ()) ;
        }

    }
    printf("\n");
}

void printIntMatrix(GrB_Matrix M) {
    GrB_Index size;
    GrB_Matrix_nrows(&size, M);
    unsigned int element;
    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            GrB_Info info = GrB_Matrix_extractElement(&element, M, i, j);

            if (info == GrB_SUCCESS)
            {
                printf(RED "%u " RESET, element);
            }
            else if (info == GrB_NO_VALUE)
            {
                // It is up to the user to determine what 'no value'
                // means.  It depends on the semiring used.
                //printf(" [no value] ");
                printf("%u ", 0);
            }
            else
            {
                printf ("Error! %s\n", GrB_error ()) ;
            }

        }
        printf("\n");
    }
    printf("\n");
}

void printDoubleVector(GrB_Vector vec) {
    GrB_Index size;
    GrB_Vector_size(&size, vec);
    double element;
    for(unsigned int i = 0; i < size; i++) {
        GrB_Info info = GrB_Vector_extractElement(&element, vec, i);

        if (info == GrB_SUCCESS)
        {
            //printf("%lf ", element);
            printf(RED "%2.2f " RESET, element);
        }
        else if (info == GrB_NO_VALUE)
        {
            // It is up to the user to determine what 'no value'
            // means.  It depends on the semiring used.
            //printf(" [no value] ");
            printf("%2.2f ", 0.0);
        }
        else
        {
            printf ("Error! %s\n", GrB_error ()) ;
        }

    }
    printf("\n");
}

void printDoubleMatrix(GrB_Matrix M) {
    GrB_Index size;
    GrB_Matrix_nrows(&size, M);
    double element;
    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            GrB_Info info = GrB_Matrix_extractElement(&element, M, i, j);

            if (info == GrB_SUCCESS)
            {
                printf(RED "%2.2f " RESET, element);
            }
            else if (info == GrB_NO_VALUE)
            {
                // It is up to the user to determine what 'no value'
                // means.  It depends on the semiring used.
                //printf(" [no value] ");
                printf("%2.2f ", 0.0);
            }
            else
            {
                printf ("Error! %s\n", GrB_error ()) ;
            }

        }
        printf("\n");
    }
    printf("\n");
}