#!/usr/bin/env bash

# for mx, we need
# matrix.mtx
# barcodes.txt
# genes.txt
# output folder
# markers.txt

#!/bin/bash

usage () {
    echo "Usage: $0 [options]
    
    Options:
    -o, --output            output folder
    -m, --matrix            matrix
    -b, --barcodes          barcodes
    -g, --genes             genes
    -e, --markers           markers
    "
    exit 1
}

while getopts ":o:m:b:g:e:" opt; do
    case $opt in
        o|--output)
            OUTPUT=$OPTARG
            ;;
        m|--matrix)
            MATRIX=$OPTARG
            ;;
        b|--barcodes)
            BARCODES=$OPTARG
            ;;
        g|--genes)
            GENES=$OPTARG
            ;;
        e|--MARKERS)
            MARKERS=$OPTARG
            ;;
        h)
            usage
            ;;
        \?)
            echo "Invalid argument"
            usage
            ;;
        :)
            echo "Add arguments"
            usage
            ;;
    esac
done

# check options        
if [ -z "$OUTPUT" -o -z "$MATRIX" -o -z "$BARCODES" -o -z "$GENES" -o -z "$MARKERS" ]
then
    echo "Error"
    usage
fi

echo "Out:" $OUTPUT
echo "MTX:" $MATRIX
echo "BCS:" $BARCODES
echo "GEN:" $GENES
echo "MAR:" $MARKERS

# make the markers file
# ../../../../code/binMarkers2ecMarkers.py ../ca/marker_matrix.csv markers.txt


# ec index (first time)
ec index -g $OUTPUT/groups.txt -t $OUTPUT/targets.txt -e $OUTPUT/ec.txt $MARKERS

# mx filter
# echo “mx filter”
# time mx filter -bi $BARCODES -bo $OUTPUT/filter_barcodes.txt -o $OUTPUT/filter.mtx $MATRIX

# normally filtering would go here copy the barcodes and matrix
cp $MATRIX $OUTPUT/filter.mtx
cp $BARCODES $OUTPUT/filter_barcodes.txt

# mx normalize
echo “mx normalize”
time mx normalize -o $OUTPUT/norm.mtx $OUTPUT/filter.mtx

# mx extract
echo “mx extract”
time mx extract -t $OUTPUT/targets.txt -gi $GENES -go $OUTPUT/extract_genes.txt -o $OUTPUT/extract.mtx $OUTPUT/norm.mtx

# mx clean
echo “mx clean”
time mx clean -gi $OUTPUT/extract_genes.txt -go $OUTPUT/clean_genes.txt -bi $OUTPUT/filter_barcodes.txt -bo $OUTPUT/clean_barcodes.txt -o $OUTPUT/clean.mtx --bad $OUTPUT/extract.mtx

# ec filter
echo “ec filter” 
time ec filter -bt $OUTPUT/clean_genes.txt.bad -o $OUTPUT/clean.markers.txt $MARKERS 

# ec index
echo “ec index”
time ec index -g $OUTPUT/clean.groups.txt -t $OUTPUT/clean.targets.txt -e $OUTPUT/clean.markers.ec $OUTPUT/clean.markers.txt

# mx extract
echo “mx extract”
time mx extract -t $OUTPUT/clean.targets.txt -gi $OUTPUT/clean_genes.txt  -go $OUTPUT/extract.clean_genes.txt -o $OUTPUT/extract.clean.mtx $OUTPUT/clean.mtx


# mx assign
echo “mx assign”
time mx assign -g $OUTPUT/clean.groups.txt -gi $OUTPUT/extract.clean_genes.txt -bi $OUTPUT/clean_barcodes.txt -e $OUTPUT/clean.markers.ec -o $OUTPUT/assignments.txt $OUTPUT/extract.clean.mtx

echo “rank”
mkdir $OUTPUT/rank
python /home/cellatlas/human/scripts/rankmtx.py $OUTPUT/extract.clean.mtx $OUTPUT/rank.mtx

echo “mx assign on rank matrix”
time mx assign -g $OUTPUT/clean.groups.txt -gi $OUTPUT/extract.clean_genes.txt -bi $OUTPUT/clean_barcodes.txt -e $OUTPUT/clean.markers.ec -o $OUTPUT/rank/assignments.txt $OUTPUT/rank.mtx


# # mx diff
#echo "mx diff"
# time mx diff -a $OUTPUT/assignments.txt -gi $GENES -b $OUTPUT/filter_barcodes.txt -o $OUTPUT/degs.txt $OUTPUT/norm.mtx
# 
# # ec mark
# echo "ec mark"
# time ec mark -p 0.05 -f 0.02 -g 5 -o $OUTPUT/markers_updated.txt $OUTPUT/degs.txt
# 
# # ec merge
# echo "ec marge"
# time ec merge -o $OUTPUT/merged.txt -m union $MARKERS $OUTPUT/markers_updated.txt

# ec index
# echo "ec index"
# time ec index -g $OUTPUT/groups.txt -t $OUTPUT/targets.txt -e $OUTPUT/ec.txt $OUTPUT/merged.txt
