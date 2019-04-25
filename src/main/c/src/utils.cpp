#include <string>

#include "utils.h"

/*
 *
 */
time_t GetCurrentMilliseconds() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

/*
 * ARGUMENT PARSING FUNCTIONS
 */

BenchmarkParameters ParseCommandLineParameters(int argc, char **argv) {
    BenchmarkParameters benchmarkParameters;

    for (int i = 0; i < argc; i++) {
        char *key = argv[i];
        char *value = argv[i + 1];

        if (strcmp(key, "--source-vertex") == 0) {
            benchmarkParameters.sourceVertex = strtoul(value, nullptr, 10);
        }
        if (strcmp(key, "--directed") == 0) {
            benchmarkParameters.directed = (strcmp(value, "true") == 0);
        }
        if (strcmp(key, "--num-vertices") == 0) {
            benchmarkParameters.numVertices = strtoul(value, nullptr, 10);
        }
        if (strcmp(key, "--dataset") == 0) {
            benchmarkParameters.inputDir = value;
        }
        if (strcmp(key, "--output") == 0) {
            benchmarkParameters.outputFile = value;
        }
    }

    return benchmarkParameters;
}