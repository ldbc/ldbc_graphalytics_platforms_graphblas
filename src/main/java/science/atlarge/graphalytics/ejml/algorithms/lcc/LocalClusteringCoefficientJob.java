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
package science.atlarge.graphalytics.ejml.algorithms.lcc;

import com.google.common.collect.BiMap;
import org.ejml.data.DMatrixD1;
import science.atlarge.graphalytics.domain.graph.Graph;
import science.atlarge.graphalytics.ejml.EjmlGraph;
import science.atlarge.graphalytics.ejml.ProcTimeLog;
import science.atlarge.graphalytics.execution.RunSpecification;
import science.atlarge.graphalytics.ejml.EjmlJob;
import science.atlarge.graphalytics.ejml.EjmlConfiguration;

import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.OutputStreamWriter;

/**
 * Local Clustering Coefficient job implementation for Ejml. This class is responsible for formatting LCC-specific
 * arguments to be passed to the platform executable, and does not include the implementation of the algorithm.
 *
 * @author Bálint Hegyi
 */
public final class LocalClusteringCoefficientJob extends EjmlJob {

    private final Graph benchmarkGraph;

    /**
     * Creates a new ConnectedComponentsJob object with all mandatory parameters specified.
     *
     * @param platformConfig the platform configuration.
     * @param inputPath      the path to the input graph.
     */
    public LocalClusteringCoefficientJob(RunSpecification runSpecification, EjmlConfiguration platformConfig,
                                         String inputPath, String outputPath) {
        super(runSpecification, platformConfig, inputPath, outputPath);
        this.benchmarkGraph = runSpecification.getBenchmarkRun().getGraph();
    }

    @Override
    public int execute() throws Exception {
        System.out.println(this.getInputPath());
        EjmlGraph graph = EjmlGraph.loadGraph(
                this.getInputPath() + "/edge.csv",
                benchmarkGraph.getSourceGraph().isDirected()
        );

        ProcTimeLog.start();
        if (benchmarkGraph.isDirected()) {
            LocalClusteringCoefficientComputation.runDirected(graph);
        } else {
            LocalClusteringCoefficientComputation.runUndirected(graph);
        }
        ProcTimeLog.end();

        BiMap<Long, Integer> mapping = graph.getMapping();
        DMatrixD1 metric = graph.getMetric();

        try (FileWriter writer = new FileWriter(this.getOutputPath())) {
            for (Long vertexId: mapping.keySet()) {
                Integer arrayId = mapping.get(vertexId);
                String edgeString = String.format("%d %e\n", vertexId, metric.get(arrayId));
                writer.write(edgeString);
            }
        }

        return 0;
    }
}
