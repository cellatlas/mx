# mx mean

```bash
mx mean [options] mx-file
```

Computes the mean of all values provides

## Options

`-o`, `--output`

Output path

## Examples

```bash
$ mx sum -a 1 matrix.mx | mx mean -p - | xargs -I {} mx norm -m pf -sf {} -o pf.mx matrix.mx
# pass the size factor to mx norm
```
