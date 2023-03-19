package science.atlarge.graphalytics.graphblas;

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.Executor;
import org.apache.commons.exec.PumpStreamHandler;
import org.apache.commons.exec.util.StringUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import science.atlarge.graphalytics.domain.benchmark.BenchmarkRun;
import science.atlarge.graphalytics.domain.graph.Graph;
import science.atlarge.graphalytics.execution.BenchmarkRunSetup;
import science.atlarge.graphalytics.execution.RunSpecification;

import java.io.IOException;
import java.nio.file.Paths;


/**
 * Base class for all jobs in the platform driver. Configures and executes a platform job using the parameters
 * and executable specified by the subclass for a specific algorithm.
 *
 * @author Bálint Hegyi
 */
public abstract class GraphblasJob {

	private static final Logger LOG = LogManager.getLogger();

	protected CommandLine commandLine;
    private final String jobId;
	private final String logPath;
	private final String inputDir;
	private final String outputFile;

	protected final RunSpecification runSpecification;
	protected final Graph benchmarkGraph;

	protected final GraphblasConfiguration platformConfig;

	/**
     * Initializes the platform job with its parameters.
	 * @param runSpecification the benchmark run specification.
	 * @param platformConfig the platform configuration.
	 * @param inputDir the file path of the input graph dataset.
	 * @param outputFile the file path of the output graph dataset.
	 */
	public GraphblasJob(RunSpecification runSpecification, GraphblasConfiguration platformConfig,
						String inputDir, String outputFile, Graph benchmarkGraph) {
		BenchmarkRun benchmarkRun = runSpecification.getBenchmarkRun();
		BenchmarkRunSetup benchmarkRunSetup = runSpecification.getBenchmarkRunSetup();

		this.jobId = benchmarkRun.getId();
		this.logPath = benchmarkRunSetup.getLogDir().resolve("platform").toString();

		this.inputDir = inputDir;
		this.outputFile = outputFile;

		this.platformConfig = platformConfig;
		this.runSpecification = runSpecification;
		this.benchmarkGraph = benchmarkGraph;
	}


	/**
	 * Executes the platform job with the pre-defined parameters.
	 *
	 * @return the exit code
	 * @throws IOException if the platform failed to run
	 */
	public int execute() throws Exception {
		String executableDir = platformConfig.getExecutablePath();
		commandLine = new CommandLine(Paths.get(executableDir).toFile());

		// List of benchmark parameters.
		String jobId = getJobId();
		String logDir = getLogPath();

		// List of dataset parameters.
		String inputDir = getInputDir();
		String outputFile = getOutputFile();

		// List of platform parameters.
		int numThreads = platformConfig.getNumThreads();

		appendBenchmarkParameters(jobId, logDir);
		appendAlgorithmParameters();
		appendDatasetParameters(inputDir, outputFile);
		appendPlatformConfigurations(numThreads);

		String commandString = StringUtils.toString(commandLine.toStrings(), " ");
		LOG.info(String.format("Execute benchmark job with command-line: [%s]", commandString));

		Executor executor = new DefaultExecutor();
		executor.setStreamHandler(new PumpStreamHandler(System.out, System.err));
		executor.setExitValue(0);
		return executor.execute(commandLine);
	}


	/**
	 * Appends the benchmark-specific parameters for the executable to a CommandLine object.
	 */
	private void appendBenchmarkParameters(String jobId, String logPath) {
		commandLine.addArgument("--job-id");
		commandLine.addArgument(jobId);

		commandLine.addArgument("--log-path");
		commandLine.addArgument(logPath);

		commandLine.addArgument("--directed");
		commandLine.addArgument(Boolean.toString(benchmarkGraph.isDirected()));
	}

	/**
	 * Appends the dataset-specific parameters for the executable to a CommandLine object.
	 */
	private void appendDatasetParameters(String inputDir, String outputFile) {
		commandLine.addArgument("--input-dir");
		commandLine.addArgument(Paths.get(inputDir).toAbsolutePath().toString());

		commandLine.addArgument("--output-file");
		commandLine.addArgument(Paths.get(outputFile).toAbsolutePath().toString());
	}


	/**
	 * Appends the platform-specific parameters for the executable to a CommandLine object.
	 */
	private void appendPlatformConfigurations(int numThreads) {
		commandLine.addArgument("--num-threads");
		commandLine.addArgument(String.valueOf(numThreads));
	}


	/**
	 * Appends the algorithm-specific parameters for the executable to a CommandLine object.
	 */
	protected abstract void appendAlgorithmParameters();

	private String getJobId() {
		return jobId;
	}

	public String getLogPath() {
		return logPath;
	}

	private String getInputDir() {
		return inputDir;
	}

	private String getOutputFile() {
		return outputFile;
	}

}
