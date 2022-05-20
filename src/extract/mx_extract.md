# mx extract

```bash
mx extract [options] mx-file
```

Extract elements from matrix that are of interest, rows / columns (with associated metadata)

## Options

`-a`, `--axis`

Extract elements from the matrix along this axis.

`-mi`, `--input-metadata-file`

Input metadata file that matches axis.

`-mo`, `--output-metadata-file`

Output metadata file that matches axis.

`-o`, `--output`

The name of the output mx file.

`-si`, `--select-idx`

File containing indices to select

`-sn`, `--select-name`

File containing names corresponding that are in input metadata file.

## Examples

```bash
$ mx extract -a 1 -mi genes.txt -mo extract_barcode.txt -o extract.mx -si select_genes_by_index.txt matrix.mx
# Selects genes that are indexed by numbers in the select_genes.txt file

$ mx extract -a 1 -mi genes.txt -mo extract_barcode.txt -o extract.mx -sn select_genes_by_name.txt matrix.mx
# Selects genes by name from the select_genes_by_name.txt file
```
