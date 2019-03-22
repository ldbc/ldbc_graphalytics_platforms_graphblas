package science.atlarge.graphalytics.ejml;

import com.google.common.collect.BiMap;
import org.ejml.data.DMatrixD1;
import org.junit.Ignore;
import org.junit.Test;
import science.atlarge.graphalytics.ejml.algorithms.lcc.LocalClusteringCoefficientComputation;
import science.atlarge.graphalytics.validation.io.GraphParser;

import java.io.FileInputStream;
import java.io.IOException;

public class EjmlGraphTests {

    @Test
    public void testMappingWithUndirected() throws IOException {
        System.out.println("Loading");
        EjmlGraph graph = EjmlGraph.loadGraph("./intermediate/example-undirected/edge.csv", false);
        System.out.println("Computing");
        LocalClusteringCoefficientComputation.runUndirected(graph);

        BiMap<Long, Integer> mapping = graph.getMapping();
        DMatrixD1 metric = graph.getMetric();

        for (long vertexId : mapping.keySet()) {
            int targetVertexId = mapping.get(vertexId);
            double lccValue = metric.get(targetVertexId);
            System.out.println(String.format("%d -> %e", vertexId, lccValue));
        }
    }

    @Test
    public void testMappingWithDirected() throws IOException {
        System.out.println("Loading");
        EjmlGraph graph = EjmlGraph.loadGraph("./intermediate/example-directed/edge.csv", true);
        System.out.println("Computing");
        LocalClusteringCoefficientComputation.runDirected(graph);

        BiMap<Long, Integer> mapping = graph.getMapping();
        DMatrixD1 metric = graph.getMetric();

        for (long vertexId : mapping.keySet()) {
            int targetVertexId = mapping.get(vertexId);
            double lccValue = metric.get(targetVertexId);
            System.out.println(String.format("%d -> %e", vertexId, lccValue));
        }
    }

}
