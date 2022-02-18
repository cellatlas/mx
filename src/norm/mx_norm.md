# mx norm

```bash
mx norm [options] mx-file
```

Normalize the matrix.

## Options

`-m`, `--method`

The method used to normalize the matrix.

`-o`, `--output`

The name of the output mx file

## Examples

```bash
$ mx norm -m log1p -o norm.mx matrix.mx
# log1p the matrix
```
