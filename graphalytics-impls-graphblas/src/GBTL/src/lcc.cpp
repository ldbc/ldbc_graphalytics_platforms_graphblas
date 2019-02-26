//#include "utils.hpp"

#include <iostream>
#include <graphblas/graphblas.hpp>

extern "C" {
#include <graphalytics_import.h>
}

#define VERBOSE
#define PRINT_RESULT

GraphBLAS::Matrix<double, GraphBLAS::UndirectedMatrixTag> FillMatrixFromFile(const char *path, size_t tuple_count) {
    printf("%s\n", path);
    GraphalyticsMatrix loaded = LoadUnweightedMatrix(path, tuple_count);

    GraphBLAS::Matrix<double, GraphBLAS::UndirectedMatrixTag> matrix{loaded.size, loaded.size};
    matrix.build(loaded.rowIndices, loaded.columnIndices, loaded.values, loaded.tuple_count);

    return matrix;
}

template<typename Type>
void WriteOutDebugVector(const char *name, const GraphBLAS::Vector<Type> &v) {
    std::cout << name << std::endl;
    std::cout << v << std::endl;
}

template<typename Type, typename Tag>
void WriteOutDebugMatrix(const char *name, const GraphBLAS::Matrix<Type, Tag> &m) {
    std::cout << name << std::endl;
    std::cout << m << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Invalid arguments" << std::endl;
        exit(-1);
    }

    // Load the matrix from file
    const char *graph_path = argv[1];
    size_t tuple_count = strtoul(argv[2], nullptr, 10);
    auto AdjM = FillMatrixFromFile(graph_path, tuple_count);

    GraphBLAS::Matrix<double, GraphBLAS::UndirectedMatrixTag> WedgeM(AdjM.nrows(), AdjM.ncols());
    GraphBLAS::mxm(WedgeM,
                   GraphBLAS::NoMask(),
                   GraphBLAS::NoAccumulate(),
                   GraphBLAS::ArithmeticSemiring<double>(),
                   AdjM, AdjM);

    GraphBLAS::Matrix<double, GraphBLAS::UndirectedMatrixTag> TriangleM(AdjM.nrows(), AdjM.ncols());
    GraphBLAS::eWiseMult(TriangleM,
                         GraphBLAS::NoMask(),
                         GraphBLAS::NoAccumulate(),
                         GraphBLAS::TimesMonoid<double>(),
                         WedgeM, AdjM);

#ifdef PRINT_RESULT
    WriteOutDebugMatrix("Result", AdjM);
    WriteOutDebugMatrix("Result", WedgeM);
    WriteOutDebugMatrix("Result", TriangleM);
#endif

}