# mx verify

```bash
mx verify [options] mx-file
```

Verify that header matches matrix, fix if not

## Options

`-f`, `--fix`

Fix the header if incorrect

`-o`, `--output`

The name of the output mx file

## Examples

```bash
$ mx verify --fix -o fixed.mx matrix.mx
# fix header of matrix.mx
```
