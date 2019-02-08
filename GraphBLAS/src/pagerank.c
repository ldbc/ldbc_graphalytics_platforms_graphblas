//------------------------------------------------------------------------------
// GraphBLAS/Demo/src/bfs_lehel.c: breadth first search using mxv with a mask
//------------------------------------------------------------------------------

#include "demos.h"

const double INITIAL_LAST_V = 100.0; // initial value for last_v to pass the while loop at first
const double EPS = 1.0e-8; // quadratic error threshold
const double DAMPING_FACTOR = 0.85;

double addition = 0.0;
double scaler = 0.0;

void scaleVector (double *z, const double *x)
{
    (*z) = (*x) * scaler;
}

void addToVector (double *z, const double *x)
{
    (*z) = (*x) + addition;
}

void power2Vector (double *z, const double *x)
{
    (*z) = pow((*x), 2);
}

int main (int argc, char **argv)
{
    GrB_Index n;
    GrB_Matrix A = NULL;
    GrB_Matrix B = NULL;
    GrB_Vector v = NULL;
    GrB_Vector last_v = NULL;
    GrB_Vector power2SumVector = NULL;
    GrB_UnaryOp scaleOp = NULL;
    GrB_UnaryOp addOp = NULL;
    GrB_UnaryOp power2Op = NULL;

    // Index and value arrays to build vectors.
    GrB_Index* I = NULL;
    double *X = NULL;
    double *XLast = NULL;

    // Reads the matrix from stdin, or from file with ./file.c < matrix.txt format.
    read_matrix(&A, stdin, false, false, false, false, true);

    // Creates a n×n matrix. n is the dimension of A (A is square matrix).
    GrB_Matrix_nrows(&n, A);

    // Creates the vectors v and last_v and power2SumVector with size n.
    GrB_Vector_new (&v, GrB_FP64, n);
    GrB_Vector_new (&last_v, GrB_FP64, n);
    GrB_Vector_new (&power2SumVector, GrB_FP64, n);

    // Creates an n-sized Index and two double arrays. Later we construct a matrices from them.
    I = malloc(n * sizeof (GrB_Index));
    X = malloc(n * sizeof (double));
    XLast = malloc(n * sizeof (double));

    // Loop from 0 to n-1 filling the Index array with the loop index, X with 1/ n and
    // XLast with 100 (INITIAL_LAST_V = 100).
    for (uint32_t i = 0; i < n; i++) {
        I[i] = i;
        X[i] = 1.0f / n;
        XLast[i] = INITIAL_LAST_V;
    }

    // Builds the v vector from the index and value arrays I and X.
    GrB_Vector_build(v, I, X, n, GrB_FIRST_FP64);

    // Builds the v_last vector from the index and value arrays I and X.
    GrB_Vector_build(last_v, I, XLast, n, GrB_FIRST_FP64);

    // Now create the initial M matrix. Let G be a n * n matrix of ones, then
    // M = d × A + ((1 - d) / n) * G.

    // User defined unary operations to multiply with or add constants to all matrix elements.
    GrB_UnaryOp_new (&scaleOp, scaleVector, GrB_FP64, GrB_FP64);
    GrB_UnaryOp_new (&addOp, addToVector, GrB_FP64, GrB_FP64);

    // Multiply with damping factor: d × A
    scaler = DAMPING_FACTOR;
    GrB_Matrix_apply(A, NULL, NULL, scaleOp, A, NULL);

    // Add ((1 - DAMPING_FACTOR) / n) * A
    addition = (1.0 - DAMPING_FACTOR) / n;
    GrB_Matrix_apply(A, NULL, NULL, addOp, A, NULL);

    // power2SumVector = last_v - v
    GrB_eWiseAdd_Vector_BinaryOp(power2SumVector, NULL, NULL, GrB_MINUS_FP64, last_v, v, NULL);

    // Sum the power of the vectors elements. To do that we create a new
    // monoid with a binary operation that sums ther 2-power of elements and
    // reduce the vector.
    GrB_UnaryOp_new (&power2Op, power2Vector, GrB_FP64, GrB_FP64);
    GrB_Vector_apply(power2SumVector, NULL, NULL, power2Op, power2SumVector, NULL);
    double quadratic_error;
    GrB_reduce(&quadratic_error, NULL, GxB_PLUS_FP64_MONOID, power2SumVector, NULL);
    quadratic_error = sqrt(quadratic_error);

    // Performs matrix vector multiplication. The vector needs to be transposed to a column vector by
    // an appropriate discriptor, but it seems to work both with or without transposing it.

    int cnt = 0;
    while (quadratic_error > EPS) {
        printf("cnt: %d\n\n", cnt);
        cnt++;

        // last_v = v;
        GrB_Vector_dup(&last_v, v);

        // A × v matrix-vector multiplication.
        GrB_mxv(v, NULL, NULL, GxB_PLUS_TIMES_FP64, A, v, NULL);

        // Count quadratic error again.
        GrB_eWiseAdd_Vector_BinaryOp(power2SumVector, NULL, NULL, GrB_MINUS_FP64, last_v, v, NULL);

        GrB_Vector_apply(power2SumVector, NULL, NULL, power2Op, power2SumVector, NULL);
        GrB_reduce(&quadratic_error, NULL, GxB_PLUS_FP64_MONOID, power2SumVector, NULL);
        quadratic_error = sqrt(quadratic_error);
    }

    // Print the result.
    double vectorElement;
    for (uint64_t i = 0; i < n; i++) {
        GrB_Vector_extractElement(&vectorElement, v, i);
        printf("%lf\n", vectorElement);
    }

    // Free GrB objects.
    GrB_free(&A);
    GrB_free(&B);
    GrB_free(&v);
    GrB_free(&last_v);
    GrB_free(&power2SumVector);
    GrB_free(&scaleOp);
    GrB_free(&addOp);
    GrB_free(&power2Op);

    // Free arrays.
    free(I);
    free(X);
    free(XLast);
}

