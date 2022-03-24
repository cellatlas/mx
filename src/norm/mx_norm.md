# mx norm

```bash
mx norm [options] mx-file
```

Normalize the matrix.

## Options

`-m`, `--method`

The method used to normalize the matrix. Options include `log1p` and `pf`. If `pf` is selected, the size factor must by provided to `-sf`.

`-sf`, `--size-factor`

Multiply all entries by this value

`-o`, `--output`

The name of the output mx file

## Examples

```bash
$ mx norm -m log1p -o norm.mx matrix.mx
# log1p the matrix
```
