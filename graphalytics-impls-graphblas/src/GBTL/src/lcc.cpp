//#include "utils.hpp"

#include <iostream>
#include <graphblas/graphblas.hpp>

extern "C" {
#include <graphalytics_import.h>
}

#define VERBOSE
#define PRINT_RESULT

GraphBLAS::Matrix<double, GraphBLAS::DirectedMatrixTag> FillMatrixFromFile(const char *path, size_t tuple_count) {
    printf("%s\n", path);
    GraphalyticsMatrix loaded = LoadUnweightedMatrix(path, tuple_count);

    GraphBLAS::Matrix<double, GraphBLAS::DirectedMatrixTag> matrix{loaded.size, loaded.size};
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
    auto A = FillMatrixFromFile(graph_path, tuple_count);

    GraphBLAS::Vector<int> v{A.nrows()};
    // Vector containing the out edges from v
    GraphBLAS::Vector<double> v_out_neighbors{A.nrows()};
    // Vector containing the in edges from v
    GraphBLAS::Vector<double> v_in_neighbors{A.nrows()};
    // Vector containing the in+out edges
    GraphBLAS::Vector<double> v_neighbors{A.nrows()};

    // Vector containing the neighbours of N(v) vertices
    GraphBLAS::Vector<double> n_neighbors{A.nrows()};
    // Vector containing the neighbours of N(v) vertices
    // but only those which are in N(v)
    GraphBLAS::Vector<double> n_masked_neighbors{A.nrows()};

    // The result vector
    GraphBLAS::Vector<double> result{A.nrows()};

    // The dimensions of the matrix
    unsigned long n = A.nrows();
    // Contains |N(v)|
    unsigned long neighbor_count;
    // Contains the edges between N(v)
    unsigned long edge_count;

    // Iterate through all vertices.
    for (unsigned long i = 0; i < n; i++) {
        std::cout << "Vertex: " << i << std::endl;
        /*
         * Calculate the neighbors of v
         */
        // Set the selector value in v
        v.setElement(i, 1);
        WriteOutDebugVector("Indicator", v);
        // Do a (v*A) to find N_out(v) aka. the outbound edges)
        GraphBLAS::vxm(v_out_neighbors,
                       GraphBLAS::NoMask(),
                       GraphBLAS::NoAccumulate(),
                       GraphBLAS::LogicalSemiring<double>(),
                       v, A);
        // Do a (A*v^t) to find N_in(v) aka. the inbound edges
        // TODO: look up if there is any bottleneck multiplying from the right side
        GraphBLAS::mxv(v_in_neighbors,
                       GraphBLAS::NoMask(),
                       GraphBLAS::NoAccumulate(),
                       GraphBLAS::LogicalSemiring<double>(),
                       A, v);
        // Reset the indicator value
        v.setElement(i, 0);
        GraphBLAS::eWiseAdd(v_neighbors,
                            GraphBLAS::NoMask(),
                            GraphBLAS::NoAccumulate(),
                            GraphBLAS::LogicalOrMonoid<double>(),
                            v_out_neighbors,
                            v_in_neighbors);
        GraphBLAS::reduce(neighbor_count,
                          GraphBLAS::NoAccumulate(),
                          GraphBLAS::PlusMonoid<double>(),
                          v_neighbors);

        /*
         * Calculate the neighbours of N(v)
         */
        GraphBLAS::vxm(n_neighbors,
                       GraphBLAS::NoMask(),
                       GraphBLAS::NoAccumulate(),
                       GraphBLAS::ArithmeticSemiring<double>(),
                       v_neighbors,
                       A);
        GraphBLAS::eWiseMult(n_masked_neighbors,
                             GraphBLAS::NoMask(),
                             GraphBLAS::NoAccumulate(),
                             GraphBLAS::TimesMonoid<double>(),
                             v_neighbors,
                             n_neighbors);

        /*
         * Calculate the neighbours of N(v)
         */
        GraphBLAS::reduce(edge_count,
                          GraphBLAS::NoAccumulate(),
                          GraphBLAS::PlusMonoid<double>(),
                          n_masked_neighbors);

#ifdef VERBOSE
        WriteOutDebugMatrix("A", A);
        WriteOutDebugVector("v_out_neighbors", v_out_neighbors);
        WriteOutDebugVector("v_in_neighbors", v_in_neighbors);
        WriteOutDebugVector("v_neighbors", v_neighbors);
        WriteOutDebugVector("n_neighbors", n_neighbors);
        WriteOutDebugVector("n_masked_neighbors", n_masked_neighbors);
        printf("\n");
#endif

        // Count lcc for the current vertex, then save it into the result vector.
        double lcc;
        if ((int) neighbor_count <= 1) {
            lcc = 0.0;
        } else {
            lcc = (double) edge_count / (neighbor_count * (neighbor_count - 1));
        }

        result.setElement(i, lcc);
    }

#ifdef PRINT_RESULT
    WriteOutDebugVector("Result", result);
#endif

}