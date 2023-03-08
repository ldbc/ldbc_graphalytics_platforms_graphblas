#pragma once

#include <vector>
#include <fstream>
#include <iostream>

#include "utils.h"

GrB_Matrix ReadMatrixMarket(const BenchmarkParameters& parameters);

std::vector<GrB_Index> ReadMapping(const BenchmarkParameters& parameters);


// sorry for this:
#define LG_FREE_ALL ;

// the macros and binread are lifted from
// https://github.com/GraphBLAS/LAGraph/blob/v1.0.1/src/benchmark/LAGraph_demo.h

// set this to 1 to check the results using a slow method
#define LG_CHECK_RESULT 0

#define DEAD_CODE -911
#define CATCH(status)                                                         \
{                                                                             \
    printf ("error: %s line: %d, status: %d\n", __FILE__, __LINE__, status) ; \
    if (msg [0] != '\0') printf ("msg: %s\n", msg) ;                          \
    LG_FREE_ALL ;                                                             \
    return (status) ;                                                         \
}

#undef  LAGRAPH_CATCH
#define LAGRAPH_CATCH(status) CATCH (status)

#undef  GRB_CATCH
#define GRB_CATCH(info) CATCH (info)

#define LAGRAPH_BIN_HEADER 512
#define LEN LAGRAPH_BIN_HEADER

#define FREAD(p,s,n)                    \
{                                       \
    if (fread (p, s, n, f) != n)        \
    {                                   \
        CATCH (-1001) ; /* file I/O error */ \
    }                                   \
}

static inline int binread   // returns 0 if successful, -1 if failure
(
    GrB_Matrix *A,          // matrix to read from the file
    FILE *f                 // file to read it from, already open
)
{
    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    char msg [LAGRAPH_MSG_LEN] ;
    msg [0] = '\0' ;

    GrB_Index *Ap = NULL, *Ai = NULL, *Ah = NULL ;
    int8_t *Ab = NULL ;
    void *Ax = NULL ;
    if (A == NULL || f == NULL) CATCH (GrB_NULL_POINTER) ;
    (*A) = NULL ;

    //--------------------------------------------------------------------------
    // basic matrix properties
    //--------------------------------------------------------------------------

    GxB_Format_Value fmt ;
    bool is_hyper, is_sparse, is_bitmap, is_full ;
    int32_t kind, typecode ;
    double hyper ;
    GrB_Type type ;
    GrB_Index nrows, ncols, nvals, nvec ;
    size_t typesize ;
    int64_t nonempty ;

    //--------------------------------------------------------------------------
    // read the header (and ignore it)
    //--------------------------------------------------------------------------

    // The header is informational only, for "head" command, so the file can
    // be visually inspected.

    char header [LAGRAPH_BIN_HEADER] ;
    FREAD (header, sizeof (char), LAGRAPH_BIN_HEADER) ;
    // printf ("%s\n", header) ;

    //--------------------------------------------------------------------------
    // read the scalar content
    //--------------------------------------------------------------------------

    FREAD (&fmt,      sizeof (GxB_Format_Value), 1) ;
    FREAD (&kind,     sizeof (int32_t), 1) ;
    FREAD (&hyper,    sizeof (double), 1) ;
    FREAD (&nrows,    sizeof (GrB_Index), 1) ;
    FREAD (&ncols,    sizeof (GrB_Index), 1) ;
    FREAD (&nonempty, sizeof (int64_t), 1) ;
    FREAD (&nvec,     sizeof (GrB_Index), 1) ;
    FREAD (&nvals,    sizeof (GrB_Index), 1) ;
    FREAD (&typecode, sizeof (int32_t), 1) ;
    FREAD (&typesize, sizeof (size_t), 1) ;

    bool iso = false ;
    if (kind > 100)
    {
        iso = true ;
        kind = kind - 100 ;
    }

    is_hyper  = (kind == 1) ;
    is_sparse = (kind == 0 || kind == GxB_SPARSE) ;
    is_bitmap = (kind == GxB_BITMAP) ;
    is_full   = (kind == GxB_FULL) ;

    switch (typecode)
    {
        case 0:  type = GrB_BOOL        ; break ;
        case 1:  type = GrB_INT8        ; break ;
        case 2:  type = GrB_INT16       ; break ;
        case 3:  type = GrB_INT32       ; break ;
        case 4:  type = GrB_INT64       ; break ;
        case 5:  type = GrB_UINT8       ; break ;
        case 6:  type = GrB_UINT16      ; break ;
        case 7:  type = GrB_UINT32      ; break ;
        case 8:  type = GrB_UINT64      ; break ;
        case 9:  type = GrB_FP32        ; break ;
        case 10: type = GrB_FP64        ; break ;
        #if 0
        case 11: type = GxB_FC32        ; break ;
        case 12: type = GxB_FC64        ; break ;
        #endif
        default: CATCH (GrB_NOT_IMPLEMENTED) ;    // unknown or unsupported type
    }

    //--------------------------------------------------------------------------
    // allocate the array content
    //--------------------------------------------------------------------------

    GrB_Index Ap_len = 0, Ap_size = 0 ;
    GrB_Index Ah_len = 0, Ah_size = 0 ;
    GrB_Index Ab_len = 0, Ab_size = 0 ;
    GrB_Index Ai_len = 0, Ai_size = 0 ;
    GrB_Index Ax_len = 0, Ax_size = 0 ;

    bool ok = true ;
    if (is_hyper)
    {
        Ap_len = nvec+1 ;
        Ah_len = nvec ;
        Ai_len = nvals ;
        Ax_len = nvals ;
        LAGraph_Malloc ((void **) &Ap, Ap_len, sizeof (GrB_Index), msg) ;
        LAGraph_Malloc ((void **) &Ah, Ah_len, sizeof (GrB_Index), msg) ;
        LAGraph_Malloc ((void **) &Ai, Ai_len, sizeof (GrB_Index), msg) ;
        Ap_size = Ap_len * sizeof (GrB_Index) ;
        Ah_size = Ah_len * sizeof (GrB_Index) ;
        Ai_size = Ai_len * sizeof (GrB_Index) ;
        ok = (Ap != NULL && Ah != NULL && Ai != NULL) ;
    }
    else if (is_sparse)
    {
        Ap_len = nvec+1 ;
        Ai_len = nvals ;
        Ax_len = nvals ;
        LAGraph_Malloc ((void **) &Ap, Ap_len, sizeof (GrB_Index), msg) ;
        LAGraph_Malloc ((void **) &Ai, Ai_len, sizeof (GrB_Index), msg) ;
        Ap_size = Ap_len * sizeof (GrB_Index) ;
        Ai_size = Ai_len * sizeof (GrB_Index) ;
        ok = (Ap != NULL && Ai != NULL) ;
    }
    else if (is_bitmap)
    {
        Ab_len = nrows*ncols ;
        Ax_len = nrows*ncols ;
        LAGraph_Malloc ((void **) &Ab, nrows*ncols, sizeof (int8_t), msg) ;
        Ab_size = Ab_len * sizeof (GrB_Index) ;
        ok = (Ab != NULL) ;
    }
    else if (is_full)
    {
        Ax_len = nrows*ncols ;
    }
    else
    {
        CATCH (DEAD_CODE) ;    // this "cannot" happen
    }
    LAGraph_Malloc ((void **) &Ax, iso ? 1 : Ax_len, typesize, msg) ;
    Ax_size = (iso ? 1 : Ax_len) * typesize ;
    ok = ok && (Ax != NULL) ;
    if (!ok) CATCH (GrB_OUT_OF_MEMORY) ;        // out of memory

    //--------------------------------------------------------------------------
    // read the array content
    //--------------------------------------------------------------------------

    if (is_hyper)
    {
        FREAD (Ap, sizeof (GrB_Index), Ap_len) ;
        FREAD (Ah, sizeof (GrB_Index), Ah_len) ;
        FREAD (Ai, sizeof (GrB_Index), Ai_len) ;
    }
    else if (is_sparse)
    {
        FREAD (Ap, sizeof (GrB_Index), Ap_len) ;
        FREAD (Ai, sizeof (GrB_Index), Ai_len) ;
    }
    else if (is_bitmap)
    {
        FREAD (Ab, sizeof (int8_t), Ab_len) ;
    }

    FREAD (Ax, typesize, (iso ? 1 : Ax_len)) ;

    //--------------------------------------------------------------------------
    // import the matrix
    //--------------------------------------------------------------------------

    if (fmt == GxB_BY_COL && is_hyper)
    {
        // hypersparse CSC
        GRB_TRY (GxB_Matrix_import_HyperCSC (A, type, nrows, ncols,
            &Ap, &Ah, &Ai, &Ax, Ap_size, Ah_size, Ai_size, Ax_size,
            iso, nvec, false, NULL)) ;
    }
    else if (fmt == GxB_BY_ROW && is_hyper)
    {
        // hypersparse CSR
        GRB_TRY (GxB_Matrix_import_HyperCSR (A, type, nrows, ncols,
            &Ap, &Ah, &Ai, &Ax, Ap_size, Ah_size, Ai_size, Ax_size,
            iso, nvec, false, NULL)) ;
    }
    else if (fmt == GxB_BY_COL && is_sparse)
    {
        // standard CSC
        GRB_TRY (GxB_Matrix_import_CSC (A, type, nrows, ncols,
            &Ap, &Ai, &Ax, Ap_size, Ai_size, Ax_size,
            iso, false, NULL)) ;
    }
    else if (fmt == GxB_BY_ROW && is_sparse)
    {
        // standard CSR
        GRB_TRY (GxB_Matrix_import_CSR (A, type, nrows, ncols,
            &Ap, &Ai, &Ax, Ap_size, Ai_size, Ax_size,
            iso, false, NULL)) ;
    }
    else if (fmt == GxB_BY_COL && is_bitmap)
    {
        // bitmap by col
        GRB_TRY (GxB_Matrix_import_BitmapC (A, type, nrows, ncols,
            &Ab, &Ax, Ab_size, Ax_size,
            iso, nvals, NULL)) ;
    }
    else if (fmt == GxB_BY_ROW && is_bitmap)
    {
        // bitmap by row
        GRB_TRY (GxB_Matrix_import_BitmapR (A, type, nrows, ncols,
            &Ab, &Ax, Ab_size, Ax_size,
            iso, nvals, NULL)) ;
    }
    else if (fmt == GxB_BY_COL && is_full)
    {
        // full by col
        GRB_TRY (GxB_Matrix_import_FullC (A, type, nrows, ncols,
            &Ax, Ax_size,
            iso, NULL)) ;
    }
    else if (fmt == GxB_BY_ROW && is_full)
    {
        // full by row
        GRB_TRY (GxB_Matrix_import_FullR (A, type, nrows, ncols,
            &Ax, Ax_size,
            iso, NULL)) ;
    }
    else
    {
        CATCH (DEAD_CODE) ;    // this "cannot" happen
    }

    GRB_TRY (GxB_Matrix_Option_set (*A, GxB_HYPER_SWITCH, hyper)) ;
    return (GrB_SUCCESS) ;
}

//------------------------------------------------------------------------------
// binwrite: write a matrix to a binary file
//------------------------------------------------------------------------------

#undef LG_FREE_ALL
#define LG_FREE_ALL                         \
{                                           \
    GrB_Matrix_free (A) ;                   \
    LAGraph_Free ((void **) &Ap, NULL) ;    \
    LAGraph_Free ((void **) &Ab, NULL) ;    \
    LAGraph_Free ((void **) &Ah, NULL) ;    \
    LAGraph_Free ((void **) &Ai, NULL) ;    \
    LAGraph_Free ((void **) &Ax, NULL) ;    \
}

#define FWRITE(p,s,n)                   \
{                                       \
    if (fwrite (p, s, n, f) != n)       \
    {                                   \
        CATCH (LAGRAPH_IO_ERROR) ;      \
    }                                   \
}

static inline int binwrite  // returns 0 if successful, < 0 on error
(
    GrB_Matrix *A,          // matrix to write to the file
    FILE *f,                // file to write it to
    const char *comments    // comments to add to the file, up to 210 characters
                            // in length, not including the terminating null
                            // byte. Ignored if NULL.  Characters past
                            // the 210 limit are silently ignored.
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    char msg [LAGRAPH_MSG_LEN] ;
    msg [0] = '\0' ;

#if !LAGRAPH_SUITESPARSE
    printf ("SuiteSparse:GraphBLAS required to write binary *.grb files\n") ;
    return (GrB_NOT_IMPLEMENTED) ;
#else

    GrB_Index *Ap = NULL, *Ai = NULL, *Ah = NULL ;
    void *Ax = NULL ;
    int8_t *Ab = NULL ;
    if (A == NULL || *A == NULL || f == NULL) CATCH (GrB_NULL_POINTER) ;

    GRB_TRY (GrB_Matrix_wait (*A, GrB_MATERIALIZE)) ;

    //--------------------------------------------------------------------------
    // determine the basic matrix properties
    //--------------------------------------------------------------------------

    GxB_Format_Value fmt ;
    GRB_TRY (GxB_Matrix_Option_get (*A, GxB_FORMAT, &fmt)) ;

    bool is_hyper = false ;
    bool is_sparse = false ;
    bool is_bitmap = false ;
    bool is_full  = false ;
    GRB_TRY (GxB_Matrix_Option_get (*A, GxB_IS_HYPER, &is_hyper)) ;
    int32_t kind ;
    double hyper;

    GRB_TRY (GxB_Matrix_Option_get (*A, GxB_HYPER_SWITCH, &hyper)) ;
    GRB_TRY (GxB_Matrix_Option_get (*A, GxB_SPARSITY_STATUS, &kind)) ;

    switch (kind)
    {
        default :
        case 0 : // for backward compatibility with prior versions
        case 2 : is_sparse = true ; break ; // GxB_SPARSE = 2
        case 1 : is_hyper  = true ; break ; // GxB_HYPERSPARSE = 1
        case 4 : is_bitmap = true ; break ; // GxB_BITMAP = 4
        case 8 : is_full   = true ; break ; // GxB_FULL = 4
    }

    //--------------------------------------------------------------------------
    // export the matrix
    //--------------------------------------------------------------------------

    GrB_Type type ;
    GrB_Index nrows, ncols, nvals, nvec ;
    GRB_TRY (GrB_Matrix_nvals (&nvals, *A)) ;
    size_t typesize ;
    int64_t nonempty = -1 ;
    const char *fmt_string ;
    bool jumbled, iso ;
    GrB_Index Ap_size, Ah_size, Ab_size, Ai_size, Ax_size ;

    if (fmt == GxB_BY_COL && is_hyper)
    {
        // hypersparse CSC
        GRB_TRY (GxB_Matrix_export_HyperCSC (A, &type, &nrows, &ncols,
            &Ap, &Ah, &Ai, &Ax, &Ap_size, &Ah_size, &Ai_size, &Ax_size,
            &iso, &nvec, &jumbled, NULL)) ;
        fmt_string = "HCSC" ;
    }
    else if (fmt == GxB_BY_ROW && is_hyper)
    {
        // hypersparse CSR
        GRB_TRY (GxB_Matrix_export_HyperCSR (A, &type, &nrows, &ncols,
            &Ap, &Ah, &Ai, &Ax, &Ap_size, &Ah_size, &Ai_size, &Ax_size,
            &iso, &nvec, &jumbled, NULL)) ;
        fmt_string = "HCSR" ;
    }
    else if (fmt == GxB_BY_COL && is_sparse)
    {
        // standard CSC
        GRB_TRY (GxB_Matrix_export_CSC (A, &type, &nrows, &ncols,
            &Ap, &Ai, &Ax, &Ap_size, &Ai_size, &Ax_size,
            &iso, &jumbled, NULL)) ;
        nvec = ncols ;
        fmt_string = "CSC " ;
    }
    else if (fmt == GxB_BY_ROW && is_sparse)
    {
        // standard CSR
        GRB_TRY (GxB_Matrix_export_CSR (A, &type, &nrows, &ncols,
            &Ap, &Ai, &Ax, &Ap_size, &Ai_size, &Ax_size,
            &iso, &jumbled, NULL)) ;
        nvec = nrows ;
        fmt_string = "CSR " ;
    }
    else if (fmt == GxB_BY_COL && is_bitmap)
    {
        // bitmap by col
        GRB_TRY (GxB_Matrix_export_BitmapC (A, &type, &nrows, &ncols,
            &Ab, &Ax, &Ab_size, &Ax_size,
            &iso, &nvals, NULL)) ;
        nvec = ncols ;
        fmt_string = "BITMAPC" ;
    }
    else if (fmt == GxB_BY_ROW && is_bitmap)
    {
        // bitmap by row
        GRB_TRY (GxB_Matrix_export_BitmapR (A, &type, &nrows, &ncols,
            &Ab, &Ax, &Ab_size, &Ax_size,
            &iso, &nvals, NULL)) ;
        nvec = nrows ;
        fmt_string = "BITMAPR" ;
    }
    else if (fmt == GxB_BY_COL && is_full)
    {
        // full by col
        GRB_TRY (GxB_Matrix_export_FullC (A, &type, &nrows, &ncols,
            &Ax, &Ax_size,
            &iso, NULL)) ;
        nvec = ncols ;
        fmt_string = "FULLC" ;
    }
    else if (fmt == GxB_BY_ROW && is_full)
    {
        // full by row
        GRB_TRY (GxB_Matrix_export_FullR (A, &type, &nrows, &ncols,
            &Ax, &Ax_size,
            &iso, NULL)) ;
        nvec = nrows ;
        fmt_string = "FULLC" ;
    }
    else
    {
        CATCH (DEAD_CODE) ;    // this "cannot" happen
    }

    //--------------------------------------------------------------------------
    // create the type string
    //--------------------------------------------------------------------------

    GRB_TRY (GxB_Type_size (&typesize, type)) ;

    char type_name [LEN] ;
    int32_t typecode ;
    if      (type == GrB_BOOL  )
    {
        snprintf (type_name, LEN, "GrB_BOOL  ") ;
        typecode = 0 ;
    }
    else if (type == GrB_INT8  )
    {
        snprintf (type_name, LEN, "GrB_INT8  ") ;
        typecode = 1 ;
    }
    else if (type == GrB_INT16 )
    {
        snprintf (type_name, LEN, "GrB_INT16 ") ;
        typecode = 2 ;
    }
    else if (type == GrB_INT32 )
    {
        snprintf (type_name, LEN, "GrB_INT32 ") ;
        typecode = 3 ;
    }
    else if (type == GrB_INT64 )
    {
        snprintf (type_name, LEN, "GrB_INT64 ") ;
        typecode = 4 ;
    }
    else if (type == GrB_UINT8 )
    {
        snprintf (type_name, LEN, "GrB_UINT8 ") ;
        typecode = 5 ;
    }
    else if (type == GrB_UINT16)
    {
        snprintf (type_name, LEN, "GrB_UINT16") ;
        typecode = 6 ;
    }
    else if (type == GrB_UINT32)
    {
        snprintf (type_name, LEN, "GrB_UINT32") ;
        typecode = 7 ;
    }
    else if (type == GrB_UINT64)
    {
        snprintf (type_name, LEN, "GrB_UINT64") ;
        typecode = 8 ;
    }
    else if (type == GrB_FP32  )
    {
        snprintf (type_name, LEN, "GrB_FP32  ") ;
        typecode = 9 ;
    }
    else if (type == GrB_FP64  )
    {
        snprintf (type_name, LEN, "GrB_FP64  ") ;
        typecode = 10 ;
    }
    else
    {
        // unsupported type (GxB_FC32 and GxB_FC64 not yet supported)
        CATCH (GrB_NOT_IMPLEMENTED) ;
    }
    type_name [72] = '\0' ;

    //--------------------------------------------------------------------------
    // write the header in ascii
    //--------------------------------------------------------------------------

    // The header is informational only, for "head" command, so the file can
    // be visually inspected.

    char version [LEN] ;
    snprintf (version, LEN, "%d.%d.%d (LAGraph DRAFT)",
        GxB_IMPLEMENTATION_MAJOR,
        GxB_IMPLEMENTATION_MINOR,
        GxB_IMPLEMENTATION_SUB) ;
    version [25] = '\0' ;

    char user [LEN] ;
    for (int k = 0 ; k < LEN ; k++) user [k] = ' ' ;
    user [0] = '\n' ;
    if (comments != NULL)
    {
        strncpy (user, comments, 210) ;
    }
    user [210] = '\0' ;

    char header [LAGRAPH_BIN_HEADER] ;
    int32_t len = snprintf (header, LAGRAPH_BIN_HEADER,
        "SuiteSparse:GraphBLAS matrix\nv%-25s\n"
        "nrows:  %-18" PRIu64 "\n"
        "ncols:  %-18" PRIu64 "\n"
        "nvec:   %-18" PRIu64 "\n"
        "nvals:  %-18" PRIu64 "\n"
        "format: %-8s\n"
        "size:   %-18" PRIu64 "\n"
        "type:   %-72s\n"
        "iso:    %1d\n"
        "%-210s\n\n",
        version, nrows, ncols, nvec, nvals, fmt_string, (uint64_t) typesize,
        type_name, iso, user) ;

    // printf ("header len %d\n", len) ;
    for (int32_t k = len ; k < LAGRAPH_BIN_HEADER ; k++) header [k] = ' ' ;
    header [LAGRAPH_BIN_HEADER-1] = '\0' ;
    FWRITE (header, sizeof (char), LAGRAPH_BIN_HEADER) ;

    //--------------------------------------------------------------------------
    // write the scalar content
    //--------------------------------------------------------------------------

    if (iso)
    {
        // kind is 1, 2, 4, or 8: add 100 if the matrix is iso
        kind = kind + 100 ;
    }

    FWRITE (&fmt,      sizeof (GxB_Format_Value), 1) ;
    FWRITE (&kind,     sizeof (int32_t), 1) ;
    FWRITE (&hyper,    sizeof (double), 1) ;
    FWRITE (&nrows,    sizeof (GrB_Index), 1) ;
    FWRITE (&ncols,    sizeof (GrB_Index), 1) ;
    FWRITE (&nonempty, sizeof (int64_t), 1) ;
    FWRITE (&nvec,     sizeof (GrB_Index), 1) ;
    FWRITE (&nvals,    sizeof (GrB_Index), 1) ;
    FWRITE (&typecode, sizeof (int32_t), 1) ;
    FWRITE (&typesize, sizeof (size_t), 1) ;

    //--------------------------------------------------------------------------
    // write the array content
    //--------------------------------------------------------------------------

    if (is_hyper)
    {
        FWRITE (Ap, sizeof (GrB_Index), nvec+1) ;
        FWRITE (Ah, sizeof (GrB_Index), nvec) ;
        FWRITE (Ai, sizeof (GrB_Index), nvals) ;
        FWRITE (Ax, typesize, (iso ? 1 : nvals)) ;
    }
    else if (is_sparse)
    {
        FWRITE (Ap, sizeof (GrB_Index), nvec+1) ;
        FWRITE (Ai, sizeof (GrB_Index), nvals) ;
        FWRITE (Ax, typesize, (iso ? 1 : nvals)) ;
    }
    else if (is_bitmap)
    {
        FWRITE (Ab, sizeof (int8_t), nrows*ncols) ;
        FWRITE (Ax, typesize, (iso ? 1 : (nrows*ncols))) ;
    }
    else
    {
        FWRITE (Ax, typesize, (iso ? 1 : (nrows*ncols))) ;
    }

    //--------------------------------------------------------------------------
    // re-import the matrix
    //--------------------------------------------------------------------------

    if (fmt == GxB_BY_COL && is_hyper)
    {
        // hypersparse CSC
        GRB_TRY (GxB_Matrix_import_HyperCSC (A, type, nrows, ncols,
            &Ap, &Ah, &Ai, &Ax, Ap_size, Ah_size, Ai_size, Ax_size,
            iso, nvec, jumbled, NULL)) ;
    }
    else if (fmt == GxB_BY_ROW && is_hyper)
    {
        // hypersparse CSR
        GRB_TRY (GxB_Matrix_import_HyperCSR (A, type, nrows, ncols,
            &Ap, &Ah, &Ai, &Ax, Ap_size, Ah_size, Ai_size, Ax_size,
            iso, nvec, jumbled, NULL)) ;
    }
    else if (fmt == GxB_BY_COL && is_sparse)
    {
        // standard CSC
        GRB_TRY (GxB_Matrix_import_CSC (A, type, nrows, ncols,
            &Ap, &Ai, &Ax, Ap_size, Ai_size, Ax_size,
            iso, jumbled, NULL)) ;
    }
    else if (fmt == GxB_BY_ROW && is_sparse)
    {
        // standard CSR
        GRB_TRY (GxB_Matrix_import_CSR (A, type, nrows, ncols,
            &Ap, &Ai, &Ax, Ap_size, Ai_size, Ax_size,
            iso, jumbled, NULL)) ;
    }
    else if (fmt == GxB_BY_COL && is_bitmap)
    {
        // bitmap by col
        GRB_TRY (GxB_Matrix_import_BitmapC (A, type, nrows, ncols,
            &Ab, &Ax, Ab_size, Ax_size,
            iso, nvals, NULL)) ;
    }
    else if (fmt == GxB_BY_ROW && is_bitmap)
    {
        // bitmap by row
        GRB_TRY (GxB_Matrix_import_BitmapR (A, type, nrows, ncols,
            &Ab, &Ax, Ab_size, Ax_size,
            iso, nvals, NULL)) ;
    }
    else if (fmt == GxB_BY_COL && is_full)
    {
        // full by col
        GRB_TRY (GxB_Matrix_import_FullC (A, type, nrows, ncols,
            &Ax, Ax_size,
            iso, NULL)) ;
    }
    else if (fmt == GxB_BY_ROW && is_full)
    {
        // full by row
        GRB_TRY (GxB_Matrix_import_FullR (A, type, nrows, ncols,
            &Ax, Ax_size,
            iso, NULL)) ;
    }
    else
    {
        CATCH (DEAD_CODE) ;    // this "cannot" happen
    }

    GRB_TRY (GxB_Matrix_Option_set (*A, GxB_HYPER_SWITCH, hyper)) ;
    return (GrB_SUCCESS) ;
#endif
}
