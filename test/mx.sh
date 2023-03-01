#!/usr/bin/env bash

# ./mx.sh -o ./out -m matrix.mtx -b barcodes.txt -g genes.txt -e markers.txt

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

mkdir -p $OUTPUT

# make the markers file
# ../../../../code/binMarkers2ecMarkers.py ../ca/marker_matrix.csv markers.txt


# ec index (first time)
ec index -g $OUTPUT/groups.txt -t $OUTPUT/targets.txt -e $OUTPUT/ec.txt $MARKERS

# mx filter
# echo "mx filter"
time mx filter -bi $BARCODES -bo $OUTPUT/filter_barcodes.txt -o $OUTPUT/filter.mtx $MATRIX

# normally filtering would go here copy the barcodes and matrix
# cp $MATRIX $OUTPUT/filter.mtx
# cp $BARCODES $OUTPUT/filter_barcodes.txt

# mx normalize
echo "mx normalize"
time mx normalize -o $OUTPUT/norm.mtx $OUTPUT/filter.mtx

# mx extract
echo "mx extract"
time mx extract -t $OUTPUT/targets.txt -gi $GENES -go $OUTPUT/extract_genes.txt -o $OUTPUT/extract.mtx $OUTPUT/norm.mtx

# mx clean
echo "mx clean"
time mx clean -gi $OUTPUT/extract_genes.txt -go $OUTPUT/clean_genes.txt -bi $OUTPUT/filter_barcodes.txt -bo $OUTPUT/clean_barcodes.txt -o $OUTPUT/clean.mtx $OUTPUT/extract.mtx

# mx assign
echo "mx assign"
time mx assign -g $OUTPUT/groups.txt -gi $OUTPUT/clean_genes.txt -bi $OUTPUT/clean_barcodes.txt -e $OUTPUT/ec.txt -o $OUTPUT/assignments.txt $OUTPUT/clean.mtx

# mx diff
echo "mx diff"
time mx diff -a $OUTPUT/assignments.txt -gi $GENES -b $OUTPUT/filter_barcodes.txt -o $OUTPUT/degs.txt $OUTPUT/norm.mtx

# ec mark
echo "ec mark"
time ec mark -p 0.05 -f 0.02 -g 5 -m 10 -o $OUTPUT/markers_updated.txt $OUTPUT/degs.txt

# ec merge
echo "ec marge"
time ec merge -o $OUTPUT/merged.txt -m union $MARKERS $OUTPUT/markers_updated.txt

# ec index
echo "ec index"
time ec index -g $OUTPUT/groups.txt -t $OUTPUT/targets.txt -e $OUTPUT/ec.txt $OUTPUT/merged.txt
