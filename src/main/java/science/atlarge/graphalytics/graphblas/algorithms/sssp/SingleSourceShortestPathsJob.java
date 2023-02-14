package science.atlarge.graphalytics.graphblas.algorithms.sssp;

import science.atlarge.graphalytics.domain.algorithms.SingleSourceShortestPathsParameters;
import science.atlarge.graphalytics.domain.graph.Graph;
import science.atlarge.graphalytics.execution.RunSpecification;
import science.atlarge.graphalytics.graphblas.GraphblasConfiguration;
import science.atlarge.graphalytics.graphblas.GraphblasJob;

/**
 * Single Source Shortest Paths job implementation for GraphBLAS. This class is responsible for formatting SSSP-specific
 * arguments to be passed to the platform executable, and does not include the implementation of the algorithm.
 *
 * @author Bálint Hegyi
 */
public final class SingleSourceShortestPathsJob extends GraphblasJob {

	/**
	 * Creates a new SingleSourceShortestPathsJob object with all mandatory parameters specified.
	 *  @param platformConfig the platform configuration.
	 * @param inputPath the path to the input graph.
	 */
	public SingleSourceShortestPathsJob(RunSpecification runSpecification, GraphblasConfiguration platformConfig,
										String inputPath, String outputPath, Graph benchmarkGraph) {
		super(runSpecification, platformConfig, inputPath, outputPath, benchmarkGraph);
	}

	@Override
	protected void appendAlgorithmParameters() {
		commandLine.addArgument("--algorithm");
		commandLine.addArgument("sssp");

		SingleSourceShortestPathsParameters params =
				(SingleSourceShortestPathsParameters) runSpecification.getBenchmarkRun().getAlgorithmParameters();
		commandLine.addArgument("--source-vertex");
		commandLine.addArgument(Long.toString(params.getSourceVertex()));

	}
}
