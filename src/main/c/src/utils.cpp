#include <string>
#include <chrono>

#include "utils.h"

//#define VERBOSE

time_t GetCurrentMilliseconds() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

/*
 * Argument parsing functions
 */

BenchmarkParameters ParseBenchmarkParameters(int argc, char **argv) {
    BenchmarkParameters benchmark_parameters;

    for (int i = 0; i < argc; i++) {
        char *key = argv[i];
        char *value = argv[i + 1];

        if (strcmp(key, "--binary") == 0) {
            benchmark_parameters.binary = (strcmp(value, "true") == 0);
        }
        if (strcmp(key, "--input-dir") == 0) {
            benchmark_parameters.input_dir = value;
        }
        if (strcmp(key, "--directed") == 0) {
            benchmark_parameters.directed = (strcmp(value, "true") == 0);
        }
        if (strcmp(key, "--source-vertex") == 0) {
            benchmark_parameters.source_vertex = std::stoul(value);
        }
        if (strcmp(key, "--damping-factor") == 0) {
            benchmark_parameters.damping_factor = std::stod(value);
        }
        if (strcmp(key, "--max-iteration") == 0) {
            benchmark_parameters.max_iteration = std::stoi(value);
        }
        if (strcmp(key, "--output-file") == 0) {
            benchmark_parameters.output_file = value;
        }
        if (strcmp(key, "--threadnum") == 0) {
            benchmark_parameters.thread_num = std::stoul(value);
        }
    }

    return benchmark_parameters;
}

ConverterParameters ParseConverterParameters(int argc, char **argv) {
    ConverterParameters converter_parameters;

    for (int i = 0; i < argc; i++) {
        char *key = argv[i];
        char *value = argv[i + 1];

        if (strcmp(key, "--data-dir")  == 0) {
            converter_parameters.data_dir = value;
        }
    }

    return converter_parameters;
}
