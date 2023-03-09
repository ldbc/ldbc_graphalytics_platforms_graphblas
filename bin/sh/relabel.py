#!/usr/bin/env python3

import duckdb
import argparse
import random
import os

def relabel(con, graph, input_vertex_path, input_edge_path, output_path, directed, weighted):
    print("Loading...")

    if weighted:
        weight_attribute_without_type = ", weight"
        weight_attribute_with_type = ", weight DOUBLE"
        element_type = "real"
        serialize_edge_weight = "|| ' ' || weight"
        grb_type = "GrB_FP64"
    else:
        weight_attribute_without_type = ""
        weight_attribute_with_type = ""
        # bool GraphBLAS matrices use integer type and a uniform value of 1 for their entities
        element_type = "integer"
        serialize_edge_weight = "|| ' ' || 1"
        grb_type = "GrB_BOOL"

    ## configuration
    con.execute(f"SET experimental_parallel_csv=true")

    ## graph tables
    con.execute(f"CREATE OR REPLACE TABLE v (id BIGINT)")
    con.execute(f"CREATE OR REPLACE TABLE e (source BIGINT, target BIGINT{weight_attribute_with_type})")
 
    ## loading
    con.execute(f"COPY v (id) FROM '{input_vertex_path}' (DELIMITER ' ', FORMAT csv)")
    con.execute(f"COPY e (source, target{weight_attribute_without_type}) FROM '{input_edge_path}' (DELIMITER ' ', FORMAT csv)")

    print("Relabelling...")
    con.execute(f"""
        CREATE VIEW e_relabelled AS
        -- Note: rowid's indexing starts from 0,
        -- while the Matrix Market format indexes from 1, hence the '+ 1'
        SELECT source_vertex.rowid + 1 AS source, target_vertex.rowid + 1 AS target{weight_attribute_without_type}
        FROM e
        JOIN v source_vertex ON source_vertex.id = e.source
        JOIN v target_vertex ON target_vertex.id = e.target
        """)

    if directed:
        matrix_type = 'general'
    else:
        matrix_type = 'symmetric'

    print("Serializing vertex mapping...")
    con.execute(f"""
        COPY (
            SELECT v.id
            FROM v
            ORDER BY v.rowid
        )
        TO '{output_path}/graph.vtx'
        WITH (HEADER false)
        """)

    print("Serializing edge mapping...")
    con.execute(f"""
        COPY (
                SELECT '%%MatrixMarket matrix coordinate {element_type} {matrix_type}' AS s
            UNION ALL
                SELECT '%%GraphBLAS {grb_type}'
            UNION ALL
                SELECT (SELECT count(*) FROM v) || ' ' || (SELECT count(*) FROM v) || ' ' || (SELECT count(*) FROM e_relabelled)
            UNION ALL
            (
                SELECT source || ' ' || target {serialize_edge_weight}
                FROM e_relabelled
                ORDER BY source, target
            )
        )
        TO '{output_path}/graph.mtx'
        WITH (HEADER false, FORMAT csv)
        """)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--graph-name', type=str, required=True)
    parser.add_argument('--input-vertex-path', type=str, required=True)
    parser.add_argument('--input-edge-path', type=str, required=True)
    parser.add_argument('--output-path', type=str, required=True)
    parser.add_argument('--weighted', type=lambda x: (str(x).lower() in ['true','1', 'yes']), required=True)
    parser.add_argument('--directed', type=lambda x: (str(x).lower() in ['true','1', 'yes']), required=True)
    parser.add_argument('--use-disk', action='store_true',  required=False)
    args = parser.parse_args()

    if args.use_disk:
        dbfile = "test.duckdb"
        if os.path.exists(dbfile):
            os.remove(dbfile)
        con = duckdb.connect(database=dbfile)
    else:
        con = duckdb.connect(database=":memory:")

    relabel(con, \
            args.graph_name, args.input_vertex_path, args.input_edge_path, \
            args.output_path, args.directed, args.weighted)


if __name__ == "__main__":
    main()
