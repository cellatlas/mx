# mx inspect

```bash
mx inspect [options] mx-file
```

Generates a JSON file containing matrix metrics.

## Options

`-o`, `--output`

The name of the output json file

## Examples

```bash
$ mx inspect -o metrics.json matrix.mx
{
 ncells: X, #, number of cels
 ngenes: X, #, number of genes
 nzeros: X: #, number of zeros
 sparsity: 0.XX, # fraction of zeros(nzeros/(ncells*ngenes))
 total_count: XX, # sum of counts in whole matrix
 avg_per_cell: XX, # mean number of counts per cell
 avg_per_gene: XX, # mean number of counts per gene
 med_per_cell: XX, # median number of counts per cell
 med_per_gene: XX, # median number of counts per gene
 min_cell: XX, # min number of counts per cell
 max_cell: XX, # max number of counts per cell
 min_gene: XX, # min number of counts per gene
 max_gene: XX, # max number of counts per gene
 cov_gene_variance: XX, # coefficient of variation on the gene variance
 cov_cell_count: XX, # coefficient of variation on the cell count (summed over all genes)
 overdispersion: XX # overdispersion parameter (computed by fitting var = mu + alpha * mu ^2)
}
```
