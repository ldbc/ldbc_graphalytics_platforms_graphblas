/*
 * Copyright 2015 Delft University of Technology
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package science.atlarge.graphalytics.ejml.algolib.lcc;

import science.atlarge.graphalytics.ejml.EjmlGraph;
import science.atlarge.graphalytics.ejml.ValidationGraphLoader;
import science.atlarge.graphalytics.ejml.algorithms.lcc.LocalClusteringCoefficientComputation;
import science.atlarge.graphalytics.validation.GraphStructure;
import science.atlarge.graphalytics.validation.algorithms.lcc.LocalClusteringCoefficientOutput;
import science.atlarge.graphalytics.validation.algorithms.lcc.LocalClusteringCoefficientValidationTest;

import java.util.HashMap;
import java.util.Map;

/**
 * Test case for the local clustering coefficient implementation in EJML.
 */
public class LocalClusteringCoefficientComputationTest extends LocalClusteringCoefficientValidationTest {

    @Override
    public LocalClusteringCoefficientOutput executeDirectedLocalClusteringCoefficient(GraphStructure graph) {
        return executeLocalClusteringCoefficient(graph, true);
    }

    @Override
    public LocalClusteringCoefficientOutput executeUndirectedLocalClusteringCoefficient(GraphStructure graph) {
        return executeLocalClusteringCoefficient(graph, false);
    }

    private LocalClusteringCoefficientOutput executeLocalClusteringCoefficient(GraphStructure graph, boolean directed) {
        final EjmlGraph ejmlGraph = ValidationGraphLoader.loadValidationGraphToDatabase(graph);

        if (directed) {
            LocalClusteringCoefficientComputation.runDirected(ejmlGraph);
        } else {
            LocalClusteringCoefficientComputation.runUndirected(ejmlGraph);
        }

        final Map<Long, Double> output = new HashMap<>();
        for (int i = 0; i < ejmlGraph.getN(); i++) {
            output.put(
                    ejmlGraph.getMapping().inverse().get(i),
                    ejmlGraph.getMetric().get(i)
            );
        }

        return new LocalClusteringCoefficientOutput(output);
    }

}
