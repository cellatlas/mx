# mx assign

```bash
mx assign [options] mx-file
```

Assign cell types based on a marker gene ec file

## Options

`-a`, `--assignments`

File containing assignments, mahalanobis distances, and entropy of assignments.

`-mi`, `--input-metadata-files`

Comma-delimited list of metadata file.

`-m`, `--markers`

Markers ec file (index form)

`g`, `--groups`

File listing out groups (corresponding to markers ec file)

## Examples

```bash
$ mx assign -a assignments.txt -mi barcodes,genes.txt -m markers.ec -g groups.txt matrix.mx
# assign cells to groups
```
