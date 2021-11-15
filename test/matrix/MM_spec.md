# MatrixMarket

`Source: https://nvlpubs.nist.gov/nistpubs/Legacy/IR/nistir5935.pdf`

> Our philosophy is that a new matrix format need not be defined unless (a) it can save at least one-half the storage of the best alternative, or (b) it supports a format of keen interest to a large community.

All Matrix Market exchange format files contain three sections, which must appear in order:

1. Header
2. Comments
3. Data

### Header.

The first line must follow the following template:

`%%MatrixMarket object format [qualifier . . .]`

| field                    | description              | type                | value                                              |
| ------------------------ | ------------------------ | ------------------- | -------------------------------------------------- |
| magic                    | fixed magic string       | char[15]            | `%%MatrixMarket ` (the space is important!)        |
| object                   | mathematical object      | char[objectLen]     | `matrix` `vector` `directed graph`                 |
| format                   | format storage of object | char[formatLen]     | `coordinate` `array`                               |
| qualifiers[0] (field)    | type of values           | char[qualifiers[0]] | `real` `complex` `integer` `pattern`               |
| qualifiers[1] (symmetry) | symmetry of matrix       | char[qualifiers[1]] | `general` `symmetric` `skew-symmetric` `hermitian` |

_Note, `qualifiers` depend on object type._

A MatrixMarket file has different specifications depending on header values.

#### `object`

This is the type of object that the file is representing. The `matrix` option covers most use cases.

| field          | description    |
| -------------- | -------------- |
| matrix         | matrix         |
| vector         | vector         |
| directed graoh | directed graph |

#### `format`

This specifies the format of the matrix being stored. For sparse matrices, use `coordinate`.

| field      | description                                                                                                                |
| ---------- | -------------------------------------------------------------------------------------------------------------------------- |
| coordinate | General sparse matrices. Only nonzero entries are provided, and the coordinates of each nonzero entry is given explicitly. |
| array      | General dense matrices. All entries are provided in a pre-defined (column-oriented) order.                                 |

#### `qualifier(s)`

The `qualifiers` describe various properties of the `object` and its values.

`qualifiers[0]`, or `field` determines the type and number of values listed for each matrix entry.

| field   | description                                                                                                     |
| ------- | --------------------------------------------------------------------------------------------------------------- |
| real    | Matrix entries are represented by a single float.                                                               |
| complex | Matrix entries are represented by two floats, the first giving the real part and the second the imaginary part. |
| integer | Matrix entries are represented by a single integer.                                                             |
| pattern | Only the matrix nonzero pattern is provided. Matrix entries are omitted (only the nonzero pattern is supplied). |

`qualifiers[1]`, or `symmetry` determines how to interpret (specifically, how to replicate) the matrix entries.

| field     | description                                                                                                        |
| --------- | ------------------------------------------------------------------------------------------------------------------ |
| general   | The matrix has no symmetry properties (All non-square matrices are general)                                        |
| symmetric | Square matrix with aij = aji. Entries below the main diagonal are stored. The entries on the main digonal are zero |
| hermitian | Square complex matrix with aij = conjugate(aji). Only entries on or below the matrix diagonal are provided         |

### Comments.

After the header, zero or more lines of comments can be added (each with first character %)

### Data.

The remainder of the file contains the data which represent the `object`. The details of the format depend upon the type of `object`. For simplicity, each data entry should occupy one line. (Of course, a single data entry may be comprised of more than one number.)

Other rules:

- All lines are limited to 1024 characters (for purposes of line buffering).
- Blank lines may appear anywhere after the first line.
- Numeric data on a given line is separated by one or more blanks; leading and trailing blanks are ignored.
- Data items of field `real` must be represented by a floating-point decimal, optionally utilizing the E-format exponential notation common to C and Fortran.
- All indices are 1-based. That is, all rows and columns are numbered starting with 1.
- Character data may be in either upper or lower case (i.e., readers must be case insensitive.)

Example of data:

```
M N L
a b A(a,b)
a c A(a,c)
d b A(d,b)
```

### Example file

```
%%MatrixMarket matrix coordinate real general
%=================================================================================
%
% This ASCII file represents a sparse MxN matrix with L
% nonzeros in the following Matrix Market format:
%
% +----------------------------------------------+
% |%%MatrixMarket matrix coordinate real general | <--- header line
% |%                                             | <--+
% |% comments                                    |    |-- 0 or more comment lines
% |%                                             | <--+
% |    M  N  L                                   | <--- rows, columns, entries
% |    I1  J1  A(I1, J1)                         | <--+
% |    I2  J2  A(I2, J2)                         |    |
% |    I3  J3  A(I3, J3)                         |    |-- L lines
% |        . . .                                 |    |
% |    IL JL  A(IL, JL)                          | <--+
% +----------------------------------------------+
%
% Indices are 1-based, i.e. A(1,1) is the first element.
%
%=================================================================================
5 5 8
1 1 1.000e+00
2 2 1.050e+01
3 3 1.500e-02
1 4 6.000e+00
4 2 2.505e+02
4 4 -2.800e+02
4 5 3.332e+01
5 5 1.200e+01
```
