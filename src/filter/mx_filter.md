# mx filter

```bash
mx filter [options] mx-file
```

Fits a GMM to summed counts along axis and filters by maxium entropy.

## Options

`-a`, `--axis`

Axis along which to filter.

`-c`, `--components`

Number of components to filter by separated by commas. Providing multiple components indicates multiple iterations of GMM filtering.

`-mi`, `--input-metadata-file`

Input metadata file for axis.

`-mo`, `--output-metadata-file`

Ouput metadata file for axis

`-o`, `--output`

The name of the output mx file

## Examples

```bash
$ mx filter -a 0 -c 2,3 -mi barcodes.txt -mo filter_barcodes.txt -o filter.mx matrix.mx
# Filter on two components then filter the result on three components.
```
