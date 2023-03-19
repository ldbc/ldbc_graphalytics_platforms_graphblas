package science.atlarge.graphalytics.graphblas.algorithms.pr;

import science.atlarge.graphalytics.domain.algorithms.BreadthFirstSearchParameters;
import science.atlarge.graphalytics.domain.algorithms.PageRankParameters;
import science.atlarge.graphalytics.domain.graph.Graph;
import science.atlarge.graphalytics.execution.RunSpecification;
import science.atlarge.graphalytics.graphblas.GraphblasConfiguration;
import science.atlarge.graphalytics.graphblas.GraphblasJob;

/**
 * PageRank job implementation for GraphBLAS. This class is responsible for formatting PR-specific
 * arguments to be passed to the platform executable, and does not include the implementation of the algorithm.
 *
 * @author Bálint Hegyi
 */
public final class PageRankJob extends GraphblasJob {

    /**
     * Creates a new PageRankJob object with all mandatory parameters specified.
     *
     * @param platformConfig the platform configuration.
     * @param inputDir      the path to the input graph.
     */
    public PageRankJob(RunSpecification runSpecification, GraphblasConfiguration platformConfig,
                       String inputDir, String outputPath, Graph benchmarkGraph) {
        super(runSpecification, platformConfig, inputDir, outputPath, benchmarkGraph);
    }

    @Override
    protected void appendAlgorithmParameters() {
        commandLine.addArgument("--algorithm");
        commandLine.addArgument("pr");

        PageRankParameters params =
                (PageRankParameters) runSpecification.getBenchmarkRun().getAlgorithmParameters();
        commandLine.addArgument("--damping-factor");
        commandLine.addArgument(Float.toString(params.getDampingFactor()));
        commandLine.addArgument("--max-iteration");
        commandLine.addArgument(Integer.toString(params.getNumberOfIterations()));
    }
}
