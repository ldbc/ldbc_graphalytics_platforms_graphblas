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

import org.ejml.data.DMatrixD1;
import org.ejml.data.DMatrixRMaj;
import org.ejml.data.DMatrixSparseCSC;
import org.ejml.sparse.csc.CommonOps_DSCC;
import science.atlarge.graphalytics.ejml.EjmlGraph;

/**
 * Implementation of the local clustering coefficient algorithm with EJML.
 * This class is responsible for the computation, given a loaded matrix.
 */
@SuppressWarnings("Duplicates")
public class LocalClusteringCoefficientComputation {

	private static void printMatrix(String name, DMatrixSparseCSC matrix) {
		int n = matrix.getNumCols();

		System.out.println(name);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				int val = (int)matrix.get(i, j);
				if (i == j) {
					if (val != 0) {
						System.out.print("! ");
					} else {
						System.out.print("* ");
					}
				} else {
					String str = val == 0 ? "-" : Integer.toString(val);
					System.out.print(str + " ");
				}

			}
			System.out.println();
		}
	}

	private static void normalizeMatrix(DMatrixSparseCSC m) {
		for (int i = 0; i < m.nz_length; i++) {
			m.nz_values[i] = Math.min(m.nz_values[i], 1.0);
		}
	}

	/**
	 * Executes the directed local clustering coefficient algorithm
	 * by setting the LCC property on all nodes.
	 */
	public static void runUndirected(EjmlGraph graph) {
		final int n = graph.getN();
		final DMatrixSparseCSC A = graph.getA();
		final DMatrixD1 metric = graph.getMetric();

		final DMatrixSparseCSC C = new DMatrixSparseCSC(n, n);

		/*
		 * Create undirected neighbour matrix
		 */
		final DMatrixSparseCSC At = new DMatrixSparseCSC(n, n);
		System.out.println("At = T(A)");
		CommonOps_DSCC.transpose(A, At, null);

		System.out.println("C = A + At");
		CommonOps_DSCC.add(1.0, A, 1.0, At, C, null, null);
		System.out.println("C = norm(C)");
		normalizeMatrix(C);

		/*
		 * Calculate wedges
		 */

		// Row sum of A
		DMatrixRMaj Cr = new DMatrixRMaj(n);
		System.out.println("Cr = rowsum(C)");
		CommonOps_DSCC.sumRows(C, Cr);

		// Create vector W for containing number of wedges per vertex
		DMatrixRMaj W = new DMatrixRMaj(n, 1);
		System.out.println("W combination");
		for (int i = 0; i < n; i++) {
			double val = Cr.get(i);
			W.set(i, val * (val - 1));
		}

		/*
		 * Calculate triangles
		 */

		// A^2 matrix
		DMatrixSparseCSC CA = new DMatrixSparseCSC(n, n);
		System.out.println("CA = C * A");
		CommonOps_DSCC.mult(C, A, CA);

		// CAC matrix with element-wise multiplication between CA*A
		DMatrixSparseCSC CAC = new DMatrixSparseCSC(n, n);
		// CAC = CA (*) A
		System.out.println("CAC = CA * C");
		CommonOps_DSCC.elementMult(CA, C, CAC, null, null);

		// Determine triangles by A3 row sum
		DMatrixRMaj Tr = new DMatrixRMaj(n, 1);
		System.out.println("Tr = rowsum(CAC)");
		CommonOps_DSCC.sumRows(CAC, Tr);

		/*
		 * Calculate LCC
		 */
		System.out.println("LCC = lcc(W, Tr)");
		for (int i = 0; i < n; i++) {
			double wedges = W.get(i);
			double triangles = Tr.get(i);
			if (wedges == 0.0) {
				metric.set(i, 0.0);
			} else {
				metric.set(i, triangles / wedges);
			}
		}
	}

	/**
	 * Executes the undirected local clustering coefficient algorithm
	 * by setting the LCC property on all nodes.
	 */
	public static void runDirected(EjmlGraph graph) {
		// diag^-1(A*A*A)
		final int n = graph.getN();
		final DMatrixSparseCSC A = graph.getA();
		final DMatrixD1 metric = graph.getMetric();

		/*
		 * Calculate wedges
		 */

		// Row sum of A
		DMatrixRMaj Ar = new DMatrixRMaj(n);
		CommonOps_DSCC.sumRows(A, Ar);

		// Create vector W for containing number of wedges per vertex
		DMatrixRMaj W = new DMatrixRMaj(n, 1);
		for (int i = 0; i < n; i++) {
			double val = Ar.get(i);
			W.set(i, val * (val - 1));
		}

		/*
		 * Calculate triangles
		 */

		// A^2 matrix
		DMatrixSparseCSC A2 = new DMatrixSparseCSC(n, n);
		CommonOps_DSCC.mult(A, A, A2);

		// A2A matrix with element-wise multiplication between A2*A
		DMatrixSparseCSC A2A = new DMatrixSparseCSC(n, n);
		// A2A = A2 (*) A
		CommonOps_DSCC.elementMult(A2, A, A2A, null, null);

		// Determine triangles by A2A row sum
		DMatrixRMaj Tr = new DMatrixRMaj(n, 1);
		CommonOps_DSCC.sumRows(A2A, Tr);

		/*
		 * Calculate LCC
		 */
		for (int i = 0; i < n; i++) {
			double wedges = W.get(i);
			double triangles = Tr.get(i);
			if (wedges == 0.0) {
				metric.set(i, 0.0);
			} else {
				metric.set(i, triangles / wedges);
			}
		}
	}

}
