# mx split

```bash
mx split [options] mx-file
```

Splits the matrix into sub matrices given by celltype assignments

## Options

`-a`, `--assignments`

File containing celltype assignments

`-o`, `--output`

Prefix location for the matrices

## Examples

```bash
$ mx split -o ./celltypes/ -a assignments.txt matrix.mx
```
