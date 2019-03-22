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
package science.atlarge.graphalytics.ejml;

import com.google.common.collect.BiMap;
import com.google.common.collect.HashBiMap;
import org.ejml.data.DMatrixD1;
import org.ejml.data.DMatrixRMaj;
import org.ejml.data.DMatrixSparseCSC;
import org.ejml.data.DMatrixSparseTriplet;
import org.ejml.ops.ConvertDMatrixStruct;
import science.atlarge.graphalytics.validation.GraphStructure;
import science.atlarge.graphalytics.validation.io.GraphParser;

import java.io.*;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;

public class EjmlGraph {

    private static class GraphEdge {
        final long sourceId;
        final long targetId;

        public GraphEdge(long sourceId, long targetId) {
            this.sourceId = sourceId;
            this.targetId = targetId;
        }
    }

    private final DMatrixSparseCSC A;
    private final BiMap<Long, Integer> mapping;
    private final int n;
    private final DMatrixD1 metric;

    public EjmlGraph(DMatrixSparseCSC A, BiMap<Long, Integer> mapping) {
        this.A = A;
        this.mapping = mapping;
        this.n = A.getNumCols();
        this.metric = new DMatrixRMaj(n, 1);
    }

    public DMatrixSparseCSC getA() {
        return A;
    }

    public BiMap<Long, Integer> getMapping() {
        return mapping;
    }

    public int getN() {
        return n;
    }

    public DMatrixD1 getMetric() {
        return metric;
    }


    public static EjmlGraph loadGraph(String edgeListFile, boolean directed) throws IOException {
        final Set<Long> vertices = new HashSet<>();
        final List<GraphEdge> graph = new ArrayList<>();

        try (BufferedReader reader = new BufferedReader(new FileReader(edgeListFile))) {
            for (String line = reader.readLine(); line != null; line = reader.readLine()) {
                String[] components = line.trim().split(" ");
                long sourceId = Long.parseLong(components[0]);
                long targetId = Long.parseLong(components[1]);

                graph.add(new GraphEdge(sourceId, targetId));
                if (!directed) {
                    graph.add(new GraphEdge(targetId, sourceId));
                }

                vertices.add(sourceId);
                vertices.add(targetId);
            }
        }

        final BiMap<Long, Integer> mapping = HashBiMap.create();
        int mappingIndex = 0;
        for (Long vertex : vertices) {
            mapping.put(vertex, mappingIndex++);
        }

        final DMatrixSparseTriplet triplets = new DMatrixSparseTriplet(
                mapping.size(),
                mapping.size(),
                graph.size()
        );

        for (GraphEdge edge : graph) {
            triplets.addItem(
                    mapping.get(edge.sourceId),
                    mapping.get(edge.targetId),
                    1.0
            );
        }

        DMatrixSparseCSC A = ConvertDMatrixStruct.convert(triplets, (DMatrixSparseCSC) null);
        return new EjmlGraph(A, mapping);
    }
}
