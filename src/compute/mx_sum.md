# mx sum

```bash
mx sum [options] mx-file
```

Computes the sum of all values provides

## Options

`-a`, `--axis`

Axis along which to sum. Default is all values.

`-o`, `--output`

Output path

## Examples

```bash
$ mx sum -a 1 matrix.mx | mx mean -p - | xargs -I {} mx norm -m pf -sf {} -o pf.mx matrix.mx
# pass the size factor to mx norm
```
