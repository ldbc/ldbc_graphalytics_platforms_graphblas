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
package science.atlarge.graphalytics.graphblas.algorithms.lcc;
import science.atlarge.graphalytics.domain.algorithms.Algorithm;
import science.atlarge.graphalytics.domain.algorithms.BreadthFirstSearchParameters;
import science.atlarge.graphalytics.domain.graph.Graph;
import science.atlarge.graphalytics.execution.RunSpecification;
import science.atlarge.graphalytics.graphblas.GraphblasJob;
import science.atlarge.graphalytics.graphblas.GraphblasConfiguration;

/**
 * Local Clustering Coefficient job implementation for GraphBLAS. This class is responsible for formatting LCC-specific
 * arguments to be passed to the platform executable, and does not include the implementation of the algorithm.
 *
 * @author Bálint Hegyi
 */
public final class LocalClusteringCoefficientJob extends GraphblasJob {

	/**
	 * Creates a new ConnectedComponentsJob object with all mandatory parameters specified.
	 *  @param platformConfig the platform configuration.
	 * @param inputPath the path to the input graph.
	 */
	public LocalClusteringCoefficientJob(RunSpecification runSpecification, GraphblasConfiguration platformConfig,
										 String inputPath, String outputPath, Graph benchmarkGraph) {
		super(runSpecification, platformConfig, inputPath, outputPath, benchmarkGraph);
	}

	@Override
	protected void appendAlgorithmParameters() {

		commandLine.addArgument("--algorithm");
		commandLine.addArgument("lcc");

	}
}
