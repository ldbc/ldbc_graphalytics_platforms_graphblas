# C++ wrapper for the Graphalytics algorithms in LAGraph

This directory contains C++ wrapper applications for all six Graphalytis algorithms. The applications invoke the relevant methods in the [LAGraph](https://github.com/GraphBLAS/LAGraph/) library.

## Notes

Note that when using GraphBLAS from C++, you cannot rely on the `_Generic` macros as they are [not supported by the C++ compiler](https://stackoverflow.com/questions/28253867/how-to-enable-the-generic-keyword).
For example, the following line compiles:
```cpp
GrB_Vector_extractElement_UINT64(&some_uint_var, vector, index);
```
but the following does not:
```cpp
GrB_Vector_extractElement(&some_uint_var, vector, index);
```
Hence, the first version should be used.
