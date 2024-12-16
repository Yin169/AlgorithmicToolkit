#ifndef SPARSEOBJ_HPP
#define SPARSEOBJ_HPP

#include <iostream>
#include <vector>
#include <stdexcept> // For std::out_of_range

// Forward declaration of VectorObj to avoid circular dependencies
template<typename TObj>
class VectorObj;

template <typename TObj>
class SparseMatrixCSC {
public:
    int _n, _m; // Number of rows and columns
    std::vector<TObj> values;      // Non-zero values
    std::vector<int> row_indices; // Row indices of non-zeros
    std::vector<int> col_ptr;     // Column pointers

    // Constructor
    SparseMatrixCSC(int rows, int cols) : _n(rows), _m(cols) {
        col_ptr.resize(_m + 1, 0);
    }

    // Destructor
    ~SparseMatrixCSC() = default;

    // Add a value to the sparse matrix
    void addValue(int row, int col, TObj value) {
        if (col >= _m || row >= _n) {
            throw std::out_of_range("Row or column index out of range.");
        }
        if (value == 0) return; // Skip zero values
        values.push_back(value);
        row_indices.push_back(row);
        // Update the column pointer for the next column
        if (col_ptr.size() > col + 1) {
            col_ptr[col + 1]++;
        }
    }

    // Get the number of rows
    const int get_row() const { return _n; }

    // Get the number of columns
    const int get_col() const { return _m; }

    // Get a column as a VectorObj
    VectorObj<TObj> get_Col(int index) const {
        if (index < 0 || index >= _m) {
            throw std::out_of_range("Index out of range for column access.");
        }
        // Assuming VectorObj can be constructed from a pointer and size
        TObj* arr = new TObj[_n](); // Initialize with zero
        for (int i = col_ptr[index]; i < col_ptr[index + 1]; i++) {
            arr[row_indices[i]] = values[i];
        }
        return VectorObj<TObj>(arr, _n); // Assume VectorObj takes ownership of arr
    }

    // Access an element
    TObj operator()(int row, int col) const {
        if (row < 0 || row >= _n || col < 0 || col >= _m) {
            throw std::out_of_range("Index out of range for element access.");
        }
        for (int i = col_ptr[col]; i < col_ptr[col + 1]; i++) {
            if (row_indices[i] == row) {
                return values[i];
            }
        }
        return TObj(); // Return default value for zero
    }

    // Copy assignment operator
    SparseMatrixCSC& operator=(const SparseMatrixCSC& other) {
        if (this != &other) {
            _n = other._n;
            _m = other._m;
            values = other.values;
            row_indices = other.row_indices;
            col_ptr = other.col_ptr;
        }
        return *this;
    }

    // Addition operator
    SparseMatrixCSC operator+(const SparseMatrixCSC& other) const {
        if (_n != other._n || _m != other._m) {
            throw std::invalid_argument("Matrices must be the same size for addition.");
        }
        SparseMatrixCSC result(_n, _m);
        for (int col = 0; col <= _m; ++col) {
            result.col_ptr[col] = 0;
        }
        for (int i = 0; i < _m; ++i) {
            for (int p = col_ptr[i]; p < col_ptr[i + 1]; ++p) {
                int row = row_indices[p];
                TObj val = values[p];
                result.addValue(row, i, val + other(row, i));
            }
        }
        return result;
    }

    // Subtraction operator
    SparseMatrixCSC operator-(const SparseMatrixCSC& other) const {
        if (_n != other._n || _m != other._m) {
            throw std::invalid_argument("Matrices must be the same size for subtraction.");
        }
        SparseMatrixCSC result(_n, _m);
        for (int col = 0; col <= _m; ++col) {
            result.col_ptr[col] = 0;
        }
        for (int i = 0; i < _m; ++i) {
            for (int p = col_ptr[i]; p < col_ptr[i + 1]; ++p) {
                int row = row_indices[p];
                TObj val = values[p];
                result.addValue(row, i, val - other(row, i));
            }
        }
        return result;
    }

    // Multiplication operator
    SparseMatrixCSC operator*(const SparseMatrixCSC& other) const {
        if (_m != other._n) {
            throw std::invalid_argument("Number of columns in the first matrix must be equal to the number of rows in the second matrix for multiplication.");
        }
        SparseMatrixCSC result(_n, other._m);
        for (int i = 0; i < _n; ++i) {
            for (int j = 0; j < other._m; ++j) {
                TObj sum = TObj();
                for (int k = 0; k < _m; ++k) {
                    for (int p = col_ptr[k]; p < col_ptr[k + 1]; ++p) {
                        if (row_indices[p] == i) {
                            for (int q = other.col_ptr[k]; q < other.col_ptr[k + 1]; ++q) {
                                if (other.row_indices[q] == j) {
                                    sum += values[p] * other.values[q];
                                }
                            }
                        }
                    }
                }
                if (sum != TObj()) {
                    result.addValue(i, j, sum);
                }
            }
        }
        return result;
    }
};

#endif // SPARSEOBJ_HPP