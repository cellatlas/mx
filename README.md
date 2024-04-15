# mx

`mx` is command line tool (and python library) to facilitate the analysis of single-cell RNAseq data.

## Installation

## Usage

`mx` consists of 10 subcommands:

```bash
usage: mx [-h] [--verbose] <CMD> ...

mx 0.0.0: handle with mx files

positional arguments:
  <CMD>
    assign    Run assignment algorithm
    clean     Drop rows/cols with all zeros
    convert   Convert matrix
    diff      Differential Expression
    extract   Extract submatrix of genes
    filter    Filter cells from matrix
    inspect   Inspect matrix
    normalize Normalize matrix with PFlog1pPF
    plot      Plot figures for a matrix
    split     Split matrix by assignments

optional arguments:
  -h, --help  show this help message and exit
  --verbose   Print debugging information
```

`mx` operates on `mtx` files. Objects must be compatible with each other when running `mx` and the following assumptions must hold

- `matrix.mtx` is a CSR Matrix Market file (rows correspond to barcodes, columns to genes)
- `barcodes.txt` is a single column text file that must have the same length as the number of rows in `matrix.mtx`
- `genes.txt` is a single column text file that must have the same length as the number of columns in `matrix.mtx`
- `assignments.txt` is a single column text file that must have the same length as the number of barcodes in the `matrix.mtx` used in `mx assign`
- `targets.txt` a single column text file of genes corresponding to genes in the `genes.txt` file (used in the extract command)
- `groups.txt` a single column text file of cell types corresponding to the same number of rows in the `ec file` used in the `mx assign` command.

### `mx assign`: Run assignment algorithm

Run a modified Gaussian Mixture model to assign cells to cell types defined in a markers.txt file.

```bash
mx assign -g GROUPS -gi GENES_IN -bi BCS_IN -e EC -o OUTPUT matrix.mtx
```

- `-g GROUPS` is the list of cell types
- `-gi GENES_IN` is a single column text file containing a list of marker genes (same length as matrix width)
- `-bi BCS_IN` is a single column text file containing barcodes (same length as matrix length)
- `-e EC` is the mapping of celltype groups to marker genes in the `ec` format (see [ec](https://github.com/sbooeshaghi/ec/) for more information)
- `-o OUTPUT` is a path to save the output file for the assignments (one per barcode)
- `matrix.mtx` path to sparse matrix file

#### Examples

```bash
$ mx assign -g groups.txt -gi genes.txt -bi barcodes.txt -e matrix.ec -o assignments.txt matrix.mtx
```

### `mx clean`: Drop rows/cols with all zeros

Remove rows and columns with all zeros from `matrix.mtx`.

```bash
mx clean -gi GENES_IN -go GENES_OUT -bi BCS_IN -bo BCS_OUT -o OUTPUT [--bad] matrix.mtx
```

- optionally, `--bad` outputs txt file with bad genes and barcodes
- `-gi GENES_IN` is a single column text file containing genes (same length as matrix width)
- `-go GENES_OUT` is the path to save a single column text file containing genes that pass the cleaning step
- `-bi BCS_IN` is a single column text file containing barcodes (same length as matrix length)
- `-bo BCS_OUT` is the path to save a single column text file containing barcodes that pass the cleaning step
- `-o OUTPUT ` is the path to save the matrix with rows and columns that pass the cleaning step
- `matrix.mtx` path to sparse matrix file

#### Examples

```bash
$ mx clean -gi genes.txt -go clean_genes.txt -bi barcodes.txt -bo clean_barcodes.txt -o clean.mtx matrix.mtx
```

### `mx convert`: Convert matrix

Convert the text representation of the gene count matrix to an h5ad object.

```bash
mx convert -o OUTPUT -bi BARCODES_IN -gi GENES_IN [-t filetype] matrix.mtx
```

- `-o OUTPUT` is the path to save the output converted matrix file
- `-bi BARCODES_IN` is a single column text file containing barcodes (same length as matrix length)
- `-gi GENES_IN` is a single column text file containing genes (same length as matrix width)
- `-t filetype` select the filetype to convert the matrix to (defeault: `h5ad`)
- `matrix.mtx` path to sparse matrix file

#### Examples

```bash
$ mx convert -o adata.h5ad -bi barcodes.txt -gi genes.txt matrix.mtx
```

### `mx diff`: Differential Expression

Perform differential expression with a t-test given a set of cell assignments

```bash
mx diff -a ASSIGNMENTS -gi GENES_IN -bi BCS_IN -o OUTPUT matrix.mtx
```

- `-a ASSIGNMENTS` is a single column text file of barcode assignments (same length as `BCS_IN`)
- `-bi BCS_IN` is a single column text file containing barcodes (same length as matrix length)
- `-gi GENES_IN` is a single column text file containing genes (same length as matrix width)
- `-o OUTPUT` is a path to save the differential expression table
- `matrix.mtx` path to sparse matrix file

The table output by `mx diff` has the following columns:

- `index`: the cell type
- `target`: the gene being tested
- `rank`: the rank of the expression of that gene within the cell type
- `nnz`: the number of non-zero cells within that cell type for the `target` gene
- `nnz_frac`: the fraction of non-zero cells within that cell type for the `target` gene
- `mean`: the mean expression of the `target` gene within that cell type
- `log_fc`: the log fold change for the `target` gene betwen the cells in the cell type and the rest of the cells
- `p_raw`: the raw pvalue from the t-test
- `p_corr`: the bonferroni corrected pvalues

#### Examples

```bash
$ mx diff -a assignments.txt -gi genes.txt -b barcodes.txt -o diff.txt norm.mtx
```

### `mx extract`: Extract submatrix of genes

Extract a submatrix on a given list of target genes.

```bash
mx extract -t TARGETS -gi GENES_IN -go GENES_OUT -o OUTPUT matrix.mtx
```

- `-t TARGETS` is a single column text file corresponding to the list of genes you wish to have in the submatrix (extracted from the full matrix)
- `-gi GENES_IN` is a single column text file containing genes in the matrix
- `-go GENES_OUT` is the path to save a single column text file of the extracted genes (should correspond to the targets file)
- `-o OUTPUT` is the path to save the submatrix containing on the genes in the targets file
- `matrix.mtx` path to sparse matrix file

#### Examples

```bash
$ mx extract -t targets.txt -gi genes.txt -go extract_genes.txt -o extract.mtx matrix.mtx
```

### `mx filter`: Filter cells from matrix

Filter cells based on the maximum entropy of a 1-D Gaussain Mixture model.

```bash
mx filter -bi BCS_IN -bo BCS_OUT -o OUTPUT matrix.mtx
```

- `-bi BCS_IN` a single column text file containing barcodes (same length as matrix length)
- `-bo BCS_OUT` is the path to save the filtered barcodes.txt file
- `-o OUTPUT` is the path to save the matrix containing only the filtered barcodes.txt
- `matrix.mtx` path to sparse matrix file

#### Examples

```bash
$ mx filter -bi barcodes.txt -bo filter_barcodes.txt -o filter.mtx matrix.mtx
```

### `mx inspect`: Inspect matrix

Generate matrix level quality control metrics.

```bash
mx inspect -o OUTPUT -gi GENES_IN -a {rows,cols,all} -bi BCS_IN matrix.mtx
```

- `-o OUTPUT` is the path to save the inspect file (either json or txt file depending on `-a`)
- `-gi GENES_IN` a single column text file containing genes (same length as matrix width)
- `-bi BCS_IN` a single column text file containing barcodes (same length as matrix length)
- `-a {rows,cols,all}` the axis along which to inspect the matrix (default: `all`)
- `matrix.mtx` path to sparse matrix file

The output file has the following structure

- when `-a all`
  - `ncells`: number of rows in the matrix,
  - `ngenes`: number of columns in the matrix,
  - `nvals`: number of non-zero values in the matrix,
  - `density`: `nvals/(ncells*ngenes)`,
  - `avg_counts_per_cell`: average total counts per cell,
  - `avg_counts_per_gene`: average total counts per cell,
  - `avg_nnzero_per_cell`: average number of nonzero entries per cell,
  - `avg_nnzero_per_gene`: average number of nonzero entries per gene,
  - `min_cell`: minimum count for cell with fewest counts,
  - `max_cell`: maximum count for cell with most counts,
  - `total_counts`: total count sum across all entries,
  - `overdispersion`: `alpha` parameter for a polynomial fit to the mean variance relation for negative binomial data (`sigma` = `alpha`\*`mu`^2 + `mu`),
  - `s_index`: the number of cells with at least that many number of counts,
- when `-a rows/cols`
  - `counts_min`: The minimum count in that row/column
  - `counts_max`: The maximum count in that row/column
  - `counts_sum`: The sum of counts in that row/column
  - `counts_mean`: The mean of counts in that row/column
  - `counts_nnzero`: The number of non-zero entries in that row/column
  - `counts_variance`: The variance of counts in that row/column

#### Examples

```bash
# inspect columns
$ mx inspect -a cols -o md_genes.txt -gi genes.txt -bi barcodes.txt matrix.mtx

# inspect rows
$ mx inspect -a rows -o md_bcs.txt -gi genes.txt -bi barcodes.txt matrix.mtx

# inspect whole matrix
$ mx inspect -a all -o inspect.json -gi genes.txt -bi barcodes.txt matrix.mtx
```

### `mx normalize`: Normalize matrix with PFlog1pPF

Normalize the counts of the matrix.

```bash
mx normalize -o OUTPUT [-m {PF,log1pPF,rank}] matrix.mtx
```

- `-o OUTPUT` is the path to save the normalized matrix
- `[-m {PF,log1pPF,rank}]` is the method to employed to normalize the matrix
  - `PF`: proportional fitting (row normalize to unity, multiply by avg. UMI count across all cells)
  - `log1pPF`: proportional fitting followed by log1p
  - `rank`: ranking procedure along the cells
- `matrix.mtx` path to sparse matrix file

#### Examples

```bash
$ mx norm -o normalize.mtx -m log1pPF matrix.mtx
```

### `mx plot`: Plot figures for a matrix

Plot figures.

```bash
mx plot -o OUTPUT -gi GENES_IN -bi BCS_IN [-m {knee}] matrix.mtx
```

- `-o OUTPUT` is the path to write the figure (png)
- `-gi GENES_IN` a single column text file containing genes (same length as matrix width)
- `-bi BCS_IN` a single column text file containing barcodes (same length as matrix length)
- `[-m {knee}]` is the method used to generate the plot (currently only implement `knee`)
- `matrix.mtx` path to sparse matrix file

#### Examples

```bash
$ mx plot -o knee.png -m knee -gi genes.txt -bi barcodes.txt matrix.mtx
```

### `mx split`: Split matrix by assignments

Split the matrix by assignments.

```bash
mx split -a ASSIGNMENTS -gi GENES_IN -bi BCS_IN -o OUTPUT matrix.mtx
```

- `-a ASSIGNMENTS` a single column text file containing assignments (single column, same length as `BCS_IN`)
- `-gi GENES_IN` a single column text file containing genes (same length as matrix width)
- `-bi BCS_IN` a single column text file containing barcodes (same length as matrix length)
- `-o OUTPUT` a folder where matrix will be split
- `matrix.mtx` a sparse matrix file

#### Examples

```bash
$ mx split -a assignments.txt -gi genes.txt -bi barcodes.txt -o ./ matrix.mtx
```
