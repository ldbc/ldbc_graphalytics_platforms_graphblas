#!/bin/bash

# avoid going into loops without matches on the wildcard
# (by default, bash simply expands e.g. `i in *.mtx` to `i=*.mtx` if there are no such files)
shopt -s nullglob

if [[ $# -eq 0 ]] ; then
    echo 'Usage: bin/sh/link-matrix-market-graphs.sh <directory-of-matrix-market-files>'
    exit 0
fi

MM_DIR=$1

mkdir -p intermediate

# unweighted graphs
for m in $MM_DIR/*.mtx; do
    # get base filename without path
    f=$(basename -- "$m")
    # drop extension and postfix: ".mtx" is 4 characters
    g="${f::-4}"
    # get postfix, skip files that have a postfix
    p="${g:(-5)}"
    [[ $p == "-bool" ]] && continue
    [[ $p == "-fp64" ]] && continue

    # create directory and add symlinks
    mkdir -p intermediate/$g
    ln -s $MM_DIR/$g.mtx intermediate/$g/graph.mtx
    ln -s $MM_DIR/$g.vtx intermediate/$g/graph.vtx
    echo $m linked to intermediate/$g/graph.vtx
done

# unweighted variants of weighted graphs
for m in $MM_DIR/*-bool.mtx; do
    # get base filename without path
    f=$(basename -- "$m")
    
    # drop extension and postfix: "-bool.mtx" is 9 characters
    g="${f::-9}"

    # create directory and add symlinks
    mkdir -p intermediate/$g
    ln -s $MM_DIR/$g-bool.mtx intermediate/$g/graph.mtx
    ln -s $MM_DIR/$g-bool.vtx intermediate/$g/graph.vtx
    echo $m linked to intermediate/$g/graph.vtx
done

# weighted graphs
for m in $MM_DIR/*-fp64.mtx; do
    # get base filename without path
    f=$(basename -- "$m")
    
    # drop extension and postfix: "-fp64.mtx" is 9 characters
    g="${f::-9}"

    # create directory and add symlinks
    mkdir -p intermediate/$g.e_weight
    ln -s $MM_DIR/$g-fp64.mtx intermediate/$g.e_weight/graph.mtx
    ln -s $MM_DIR/$g-fp64.vtx intermediate/$g.e_weight/graph.vtx
    echo $m linked to intermediate/$g/graph.vtx
done
