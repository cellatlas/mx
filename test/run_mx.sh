#!/usr/bin/env bash

# ec index (first time)
# ec index -g groups.txt -t targets.txt -e ec.txt markers.txt

# mx filter
echo "mx filter"
time mx filter -bi barcodes.txt -bo filter_barcodes.txt -o filter.mtx matrix.mtx

# mx normalize
echo "mx normalize"
time mx normalize -o norm.mtx filter.mtx

# mx extract
echo "mx extract"
time mx extract -t targets.txt -gi genes.txt -go extract_genes.txt -o extract.mtx norm.mtx

# mx clean
echo "mx clean"
time mx clean -gi extract_genes.txt -go clean_genes.txt -bi filter_barcodes.txt -bo clean_barcodes.txt -o clean.mtx extract.mtx

# mx assign
echo "mx assign"
time mx assign -g groups.txt -gi clean_genes.txt -bi clean_barcodes.txt -e ec.txt -o assignments.txt clean.mtx

# mx diff
echo "mx diff"
time mx diff -a assignments.txt -gi genes.txt -b filter_barcodes.txt -o degs.txt norm.mtx

# ec mark
echo "ec mark"
time ec mark -p 0.05 -f 0.02 -g 5 -o markers_updated.txt degs.txt

# ec merge
echo "ec marge"
time ec merge -o merged.txt -m union markers.txt markers_updated.txt

# ec index
echo "ec index"
time ec index -g groups.txt -t targets.txt -e ec.txt merged.txt
