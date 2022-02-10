# mx clean

```bash
mx clean [options] mx-file
```

Drops rows / columns that are all zero

## Options

`-mi`, `--input-metadata-files`

Comma delimited list of metadata that matches the shape of the matrix

`-mo`, `--output-metadata-files`

Comma delimited list of metadata to be written

`-o`, `--output`

The name of the output mx file

## Examples

```bash
$ mx clean -o clean.mx matrix.mx
# drops zeros of row and column
```
