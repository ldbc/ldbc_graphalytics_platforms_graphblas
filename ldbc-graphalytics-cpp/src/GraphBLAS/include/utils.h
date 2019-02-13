#pragma once

#include "graphalytics_import.h"
#include "GraphBLAS.h"

//------------------------------------------------------------------------------
// CHECK: expr must be true; if not, return an error condition
//------------------------------------------------------------------------------

GrB_Info FillMatrixFromFile(const char *path, size_t tuple_count, GrB_Matrix *matrix);

// the #include'ing file must define the FREE_ALL macro

#define CHECK(expr,info)                                                \
{                                                                       \
    if (! (expr))                                                       \
    {                                                                   \
        /* free the result and all workspace, and return NULL */        \
        FREE_ALL ;                                                      \
        printf ("Failure: line %d file %s\n", __LINE__, __FILE__) ;     \
        return (info) ;                                                 \
    }                                                                   \
}

//------------------------------------------------------------------------------
// OK: call a GraphBLAS method and check the result
//------------------------------------------------------------------------------

// OK(method) is a macro that calls a GraphBLAS method and checks the status;
// if a failure occurs, it handles the error via the CHECK macro above, and
// returns the error status to the caller.

#define OK(method)                                                      \
{                                                                       \
    info = method ;                                                     \
    if (info != GrB_SUCCESS)                                            \
    {                                                                   \
        printf ("GraphBLAS error:\n%s\n", GrB_error ( )) ;              \
        CHECK (false, info) ;                                           \
    }                                                                   \
}
