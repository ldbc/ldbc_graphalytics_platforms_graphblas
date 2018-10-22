
#include "GraphBLAS.h"
#include "printer.h"


typedef struct {
    double val;
    int index;
} tuple;

void min_with_index(tuple* Z, const tuple* X, const tuple* Y) {
    double min = 0.0;
    int min_index = 0;
    if (X->val < Y->val) {
        min = X->val;
        min_index = X->index;
    } else {
        min = Y->val;
        min_index = Y->index;
    }
}

int main (int argc, char **argv) {

    // this is required per the spec, although SuiteSparse:GraphBLAS
    // does fine without it:
    GrB_init(GrB_NONBLOCKING);

    tuple mytuple;
    GrB_BinaryOp binOp = NULL;
    GrB_Monoid mon = NULL;
    GrB_Vector v = NULL;

    GrB_BinaryOp_new(&binOp, min_with_index, GrB_FP64, GrB_FP64, GrB_FP64);
    GrB_Monoid_new(&mon, binOp, (double) 0.0);

    GrB_Vector_new

    // main is finishing anyway, but this frees internal workspace.
    // The spec also requires it:
    GrB_finalize();
}
