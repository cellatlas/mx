#include "Matrix.h"
#include <stdexcept>

Matrix::Matrix(unsigned rows, unsigned cols)
    : rows_(rows), cols_(cols)
//, data_ ← initialized below after the if...throw statement
{
    if (rows == 0 || cols == 0)
        throw std::invalid_argument("Matrix constructor has 0 size");
    data_ = new double[rows * cols];
}
Matrix::~Matrix()
{
    delete[] data_;
}
double &Matrix::operator()(unsigned row, unsigned col)
{
    if (row >= rows_ || col >= cols_)
        throw std::invalid_argument("Matrix subscript out of bounds");
    return data_[cols_ * row + col];
}
double Matrix::operator()(unsigned row, unsigned col) const
{
    if (row >= rows_ || col >= cols_)
        throw std::invalid_argument("const Matrix subscript out of bounds");
    return data_[cols_ * row + col];
}

unsigned Matrix::getRows()
{
    return rows_;
}
unsigned Matrix::getCols()
{
    return cols_;
}

// void Matrix::Sum(int axis)
// {
//     Matrix sum()
//     if (axis == 0)
//     {
//         Matrix sum(1, cols_);
//     }
//     else
//     {
//         Matrix sum(rows_, 1);
//     }

//     for (int i = 0; i < Matrix::getRows(); i++)
//     {
//         sum()data_[cols_ * i + cols_]
//     }
// }

// Matrix Matrix::Mean(int axis){}
// Matrix Matrix::Median(int axis){}
// Matrix Matrix::Min(int axis){}
// Matrix Matrix::Max(int axis){}