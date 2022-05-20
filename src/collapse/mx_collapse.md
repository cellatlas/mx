# mx collapse

```bash
mx collapse [options] mx-file
```

Drops rows and/or columns where all elements are zero

## Options

`-a`, `--axis`

The axis to drop elements from (0: rows, 1: cols, -1: rows and cols)

`-mi`, `--input-metadata-files`

Comma delimited list of metadata that matches the shape of the matrix, ordered by axis (e.g. row,col)

`-mo`, `--output-metadata-files`

Comma delimited list of metadata to be written, ordered by axis (e.g. row,col)

`-o`, `--output`

The name of the output mx file

## Examples

```bash
$ mx collapse -a -1 -mi barcodes.txt,genes.txt -mo collapse_barcodes.txt,collapse_genes.txt -o collapse.mx matrix.mx
# drops rows and columns that sum to zeros
```
