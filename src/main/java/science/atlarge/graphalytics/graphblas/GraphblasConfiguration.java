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

import org.apache.commons.configuration.Configuration;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import science.atlarge.graphalytics.configuration.ConfigurationUtil;
import science.atlarge.graphalytics.configuration.GraphalyticsExecutionException;

import java.nio.file.Paths;

/**
 * Collection of configurable platform options.
 *
 * @author Bálint Hegyi
 */
public final class GraphblasConfiguration {

	protected static final Logger LOG = LogManager.getLogger();

	private static final String BENCHMARK_PROPERTIES_FILE = "benchmark.properties";
	private static final String HOME_PATH_KEY = "platform.graphblas.home";
	private static final String NUM_MACHINES_KEY = "platform.graphblas.num-machines";
	private static final String NUM_THREADS_KEY = "platform.graphblas.num-threads";

	private String loaderPath;
	private String unloaderPath;
	private String executablePath;
	private String terminatorPath;
	private int numMachines = 1;
	private int numThreads = 1;

	/**
	 * Creates a new GraphblasConfiguration object to capture all platform parameters that are not specific to any algorithm.
	 */
	public GraphblasConfiguration(){
	}

	public String getLoaderPath() {
		return loaderPath;
	}

	public void setLoaderPath(String loaderPath) {
		this.loaderPath = loaderPath;
	}

	public String getUnloaderPath() {
		return unloaderPath;
	}

	public void setUnloaderPath(String unloaderPath) {
		this.unloaderPath = unloaderPath;
	}

	/**
	 * @param executablePath the directory containing executables
	 */
	public void setExecutablePath(String executablePath) {
		this.executablePath = executablePath;
	}

	/**
	 * @return the directory containing executables
	 */
	public String getExecutablePath() {
		return executablePath;
	}

	public String getTerminatorPath() {
		return terminatorPath;
	}

	public void setTerminatorPath(String terminatorPath) {
		this.terminatorPath = terminatorPath;
	}

	/**
	 * @return the number of machines
	 */
	public int getNumMachines() {
		return numMachines;
	}

	/**
	 * @param numMachines the number of machines
	 */
	public void setNumMachines(int numMachines) {
		this.numMachines = numMachines;
	}

	/**
	 * @param numThreads the number of threads to use on each machine
	 */
	public void setNumThreads(int numThreads) {
		this.numThreads = numThreads;
	}

	/**
	 * @return the number of threads to use on each machine
	 */
	public int getNumThreads() {
		return numThreads;
	}


	public static GraphblasConfiguration parsePropertiesFile() {

		GraphblasConfiguration platformConfig = new GraphblasConfiguration();

		Configuration configuration = null;
		try {
			configuration = ConfigurationUtil.loadConfiguration(BENCHMARK_PROPERTIES_FILE);
		} catch (Exception e) {
			LOG.warn(String.format("Failed to load configuration from %s", BENCHMARK_PROPERTIES_FILE));
			throw new GraphalyticsExecutionException("Failed to load configuration. Benchmark run aborted.", e);
		}

		String loaderPath = Paths.get("./bin/sh/load-graph.sh").toString();
		platformConfig.setLoaderPath(loaderPath);

		String unloaderPath = Paths.get("./bin/sh/unload-graph.sh").toString();
		platformConfig.setUnloaderPath(unloaderPath);

		String executablePath = Paths.get("./bin/sh/execute-job.sh").toString();
		platformConfig.setExecutablePath(executablePath);

		String terminatorPath = Paths.get("./bin/sh/terminate-job.sh").toString();
		platformConfig.setTerminatorPath(terminatorPath);

		Integer numMachines = configuration.getInteger(NUM_MACHINES_KEY, null);
		if (numMachines != null) {
			platformConfig.setNumMachines(numMachines);
		} else {
			platformConfig.setNumMachines(1);
		}

		Integer numThreads = configuration.getInteger(NUM_THREADS_KEY, null);
		if (numThreads != null) {
			platformConfig.setNumThreads(numThreads);
		} else {
			platformConfig.setNumThreads(1);
		}

		return platformConfig;
	}

}
