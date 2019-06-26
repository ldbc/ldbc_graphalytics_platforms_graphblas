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
package science.atlarge.graphalytics.graphblas;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import science.atlarge.graphalytics.domain.algorithms.Algorithm;
import science.atlarge.graphalytics.domain.benchmark.BenchmarkRun;
import science.atlarge.graphalytics.domain.graph.FormattedGraph;
import science.atlarge.graphalytics.domain.graph.Graph;
import science.atlarge.graphalytics.domain.graph.LoadedGraph;
import science.atlarge.graphalytics.execution.*;
import science.atlarge.graphalytics.graphblas.algorithms.bfs.BreadthFirstSearchJob;
import science.atlarge.graphalytics.graphblas.algorithms.lcc.LocalClusteringCoefficientJob;
import science.atlarge.graphalytics.graphblas.algorithms.pr.PageRankJob;
import science.atlarge.graphalytics.graphblas.algorithms.sssp.SingleSourceShortestPathJob;
import science.atlarge.graphalytics.graphblas.algorithms.wcc.WeaklyConnectedComponents;
import science.atlarge.graphalytics.report.result.BenchmarkMetrics;

import java.nio.file.Path;
import java.nio.file.Paths;

/**
 * GraphBLAS platform driver for the Graphalytics benchmark.
 *
 * @author Bálint Hegyi
 */
public class GraphblasPlatform implements Platform {

	protected static final Logger LOG = LogManager.getLogger();

	public static final String PLATFORM_NAME = "graphblas";
	public GraphblasLoader loader;

	@Override
	public void verifySetup() throws Exception {

	}

	@Override
	public LoadedGraph loadGraph(FormattedGraph formattedGraph) throws Exception {
		GraphblasConfiguration platformConfig = GraphblasConfiguration.parsePropertiesFile();
		loader = new GraphblasLoader(formattedGraph, platformConfig);

		LOG.info("Loading graph " + formattedGraph.getName());
		Path loadedPath = Paths.get("./intermediate").resolve(formattedGraph.getName());

		try {

			int exitCode = loader.load(loadedPath.toString());
			if (exitCode != 0) {
				throw new PlatformExecutionException("GraphBLAS exited with an error code: " + exitCode);
			}
		} catch (Exception e) {
			throw new PlatformExecutionException("Failed to load a GraphBLAS dataset.", e);
		}
		LOG.info("Loaded graph " + formattedGraph.getName());
		return new LoadedGraph(formattedGraph, loadedPath.toString());
	}

	@Override
	public void deleteGraph(LoadedGraph loadedGraph) throws Exception {
		LOG.info("Unloading graph " + loadedGraph.getFormattedGraph().getName());
		try {

			int exitCode = loader.unload(loadedGraph.getLoadedPath());
			if (exitCode != 0) {
				throw new PlatformExecutionException("GraphBLAS exited with an error code: " + exitCode);
			}
		} catch (Exception e) {
			throw new PlatformExecutionException("Failed to unload a GraphBLAS dataset.", e);
		}
		LOG.info("Unloaded graph " +  loadedGraph.getFormattedGraph().getName());
	}

	@Override
	public void prepare(RunSpecification runSpecification) throws Exception {

	}

	@Override
	public void startup(RunSpecification runSpecification) throws Exception {
		BenchmarkRunSetup benchmarkRunSetup = runSpecification.getBenchmarkRunSetup();
		Path logDir = benchmarkRunSetup.getLogDir().resolve("platform").resolve("runner.logs");
		GraphblasCollector.startPlatformLogging(logDir);
	}

	@Override
	public void run(RunSpecification runSpecification) throws PlatformExecutionException {
		BenchmarkRun benchmarkRun = runSpecification.getBenchmarkRun();
		BenchmarkRunSetup benchmarkRunSetup = runSpecification.getBenchmarkRunSetup();
		RuntimeSetup runtimeSetup = runSpecification.getRuntimeSetup();

		Algorithm algorithm = benchmarkRun.getAlgorithm();
		GraphblasConfiguration platformConfig = GraphblasConfiguration.parsePropertiesFile();
		String inputPath = runtimeSetup.getLoadedGraph().getLoadedPath();
		String outputPath = benchmarkRunSetup.getOutputDir().resolve(benchmarkRun.getName()).toAbsolutePath().toString();
		Graph benchmarkGraph = benchmarkRun.getGraph();

		GraphblasJob job;
		switch (algorithm) {
			case LCC:
				job = new LocalClusteringCoefficientJob(runSpecification, platformConfig, inputPath, outputPath, benchmarkGraph);
				break;
			case BFS:
				job = new BreadthFirstSearchJob(runSpecification, platformConfig, inputPath, outputPath, benchmarkGraph);
				break;
			case SSSP:
				job = new SingleSourceShortestPathJob(runSpecification, platformConfig, inputPath, outputPath, benchmarkGraph);
				break;
			case PR:
				job = new PageRankJob(runSpecification, platformConfig, inputPath, outputPath, benchmarkGraph);
				break;
			case WCC:
				job = new WeaklyConnectedComponents(runSpecification, platformConfig, inputPath, outputPath, benchmarkGraph);
				break;
			default:
				throw new PlatformExecutionException("Failed to load algorithm implementation.");
		}

		LOG.info("Executing benchmark with algorithm \"{}\" on graph \"{}\".",
				benchmarkRun.getAlgorithm().getName(),
				benchmarkRun.getFormattedGraph().getName());

		try {

			int exitCode = job.execute();
			if (exitCode != 0) {
				throw new PlatformExecutionException("GraphBLAS exited with an error code: " + exitCode);
			}
		} catch (Exception e) {
			throw new PlatformExecutionException("Failed to execute a GraphBLAS job.", e);
		}

		LOG.info("Executed benchmark with algorithm \"{}\" on graph \"{}\".",
				benchmarkRun.getAlgorithm().getName(),
				benchmarkRun.getFormattedGraph().getName());

	}

	@Override
	public BenchmarkMetrics finalize(RunSpecification runSpecification) throws Exception {
		GraphblasCollector.stopPlatformLogging();
		BenchmarkRunSetup benchmarkRunSetup = runSpecification.getBenchmarkRunSetup();
		Path logDir = benchmarkRunSetup.getLogDir().resolve("platform");

		BenchmarkMetrics metrics = new BenchmarkMetrics();
		metrics.setProcessingTime(GraphblasCollector.collectProcessingTime(logDir));
		return metrics;
	}

	@Override
	public void terminate(RunSpecification runSpecification) throws Exception {
		BenchmarkRunner.terminatePlatform(runSpecification);
	}

	@Override
	public String getPlatformName() {
		return PLATFORM_NAME;
	}
}
