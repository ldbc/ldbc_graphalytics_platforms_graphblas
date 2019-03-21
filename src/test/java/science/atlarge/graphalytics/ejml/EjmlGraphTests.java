package science.atlarge.graphalytics.ejml;

import com.google.common.collect.BiMap;
import org.ejml.data.DMatrixD1;
import org.junit.Test;
import science.atlarge.graphalytics.ejml.algorithms.lcc.LocalClusteringCoefficientComputation;

import java.io.IOException;

public class EjmlGraphTests {

    @Test
    public void testMappingWithUndirected() throws IOException {
        EjmlGraph graph = EjmlGraph.loadGraph("./intermediate/example-undirected/edge.csv", false);
        LocalClusteringCoefficientComputation.runUndirected(graph);
        graph.getMetric().print();
    }

    @Test
    public void testMappingWithDirected() throws IOException {
        EjmlGraph graph = EjmlGraph.loadGraph("./intermediate/example-directed/edge.csv", true);
        LocalClusteringCoefficientComputation.runDirected(graph);

        BiMap<Long, Integer> mapping = graph.getMapping();
        DMatrixD1 metric = graph.getMetric();

        for (long vertexId : mapping.keySet()) {
            int targetVertexId = mapping.get(vertexId);
            double lccValue = metric.get(targetVertexId);
            System.out.println(String.format("%d -> %f", vertexId, lccValue));
        }
    }

}
