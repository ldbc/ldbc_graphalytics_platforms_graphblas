package science.atlarge.graphalytics.graphblas.algorithms.cdlp;

import science.atlarge.graphalytics.domain.algorithms.CommunityDetectionLPParameters;
import science.atlarge.graphalytics.domain.algorithms.PageRankParameters;
import science.atlarge.graphalytics.domain.graph.Graph;
import science.atlarge.graphalytics.execution.RunSpecification;
import science.atlarge.graphalytics.graphblas.GraphblasConfiguration;
import science.atlarge.graphalytics.graphblas.GraphblasJob;

/**
 * Community Detection by job implementation for GraphBLAS. This class is responsible for formatting CDLP-specific
 * arguments to be passed to the platform executable, and does not include the implementation of the algorithm.
 *
 * @author Bálint Hegyi
 * @author Gábor Szárnyas
 */
public final class CommunityDetectionLPJob extends GraphblasJob {

	/**
	 * Creates a new LocalClusteringCoefficientJob object with all mandatory parameters specified.
	 *  @param platformConfig the platform configuration.
	 * @param inputPath the path to the input graph.
	 */
	public CommunityDetectionLPJob(RunSpecification runSpecification, GraphblasConfiguration platformConfig,
                                   String inputPath, String outputPath, Graph benchmarkGraph) {
		super(runSpecification, platformConfig, inputPath, outputPath, benchmarkGraph);
	}

	@Override
	protected void appendAlgorithmParameters() {
		commandLine.addArgument("--algorithm");
		commandLine.addArgument("cdlp");

		CommunityDetectionLPParameters params =
				(CommunityDetectionLPParameters) runSpecification.getBenchmarkRun().getAlgorithmParameters();
		commandLine.addArgument("--max-iteration");
		commandLine.addArgument(Integer.toString(params.getMaxIterations()));
	}
}
