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
import org.ejml.data.DMatrixSparseCSC;
import org.ejml.data.DMatrixSparseTriplet;
import org.ejml.ops.ConvertDMatrixStruct;
import science.atlarge.graphalytics.util.graph.PropertyGraph;
import science.atlarge.graphalytics.validation.GraphStructure;

/**
 * Utility class for loading a validation graph into EJML matrices.
 */
public final class ValidationGraphLoader {

    private ValidationGraphLoader() {
    }

    public static EjmlGraph loadValidationGraphToDatabase(GraphStructure validationGraph) {
        final int n = validationGraph.getVertices().size();
        DMatrixSparseTriplet triplets = new DMatrixSparseTriplet(n, n, n);
        BiMap<Long, Integer> mapping = HashBiMap.create(n);

        int i = 0;
        for (long vertexId : validationGraph.getVertices()) {
            mapping.put(vertexId, i);
            i++;
        }

        for (long vertexId : validationGraph.getVertices()) {
            for (long neighbourId : validationGraph.getEdgesForVertex(vertexId)) {
                triplets.addItem(mapping.get(vertexId), mapping.get(neighbourId), 1.0);
            }
        }

        DMatrixSparseCSC A = ConvertDMatrixStruct.convert(triplets, (DMatrixSparseCSC) null);

        return new EjmlGraph(A, mapping);
    }


    public static <V, E> DMatrixSparseCSC loadValidationGraphToDatabase(PropertyGraph<V, E> graph) {
        throw new UnsupportedOperationException();
    }

}
