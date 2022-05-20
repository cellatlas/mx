// https://isocpp.org/wiki/faq/operator-overloading#matrix-subscript-op

class Matrix
{
private:
    unsigned rows_, cols_;
    double *data_;

public:
    Matrix(unsigned rows, unsigned cols);
    double &operator()(unsigned row, unsigned col);      // Subscript operators often come in pairs
    double operator()(unsigned row, unsigned col) const; // Subscript operators often come in pairs
    ~Matrix();                                           // Destructor
    Matrix(const Matrix &m);                             // Copy constructor
    Matrix &operator=(const Matrix &m);                  // Assignment operator
    unsigned getRows();
    unsigned getCols();
    void Sum(int axis);
    // Mean(int axis);
    // Median(int axis);
    // Min(int axis);
    // Max(int axis);
};