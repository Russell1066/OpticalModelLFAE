#pragma once

#define _USE_MATH_DEFINES

#include <assert.h>
#include <complex>
#include <math.h>
#include <vector>

// Incomplete class
// Does not correctly deal with Array2D const&
template<typename T>
class Array2D
{
public:
    typedef T value_type;
    typedef value_type& reference;
    typedef value_type const& const_reference;
    using vector = std::vector<value_type>;
    using iterator = typename std::vector<value_type>::iterator;
    using const_iterator = typename std::vector<value_type>::const_iterator;


    class Accessor
    {
    public:
        Accessor(vector &data, int offset, int colSize) :
            data(data), offset(offset), colSize(colSize)
        {
        }

        reference operator [](int column)
        {
            assert(column >= 0 && column < colSize);
            return data[offset + column];
        }

        iterator begin()
        {
            return data.begin() + offset;
        }

        iterator end()
        {
            return data.begin() + offset + colSize;
        }

        int size() const
        {
            return colSize;
        }

        const_reference operator [](int column) const
        {
            assert(column >= 0 && column < colSize);
            return data[offset + column];
        }

    private:
        vector &data;
        int offset;
        int colSize;
    };

    Array2D(int row, int col) : m_rows(row), m_cols(col), m_data(col * row)
    {
        assert(col > 0);
        assert(row > 0);
    }

    Accessor operator [](int row)
    {
        assert(row >= 0 && row < m_rows);
        return Accessor(m_data, row * m_cols, m_cols);
    }

    iterator begin() { return m_data.begin(); }
    iterator end() { return m_data.end(); }
    size_t size() const { return m_data.size(); }
    int cols() const { return m_cols; }
    int rows() const { return m_rows; }

    const_iterator begin() const { return m_data.begin(); }
    const_iterator end() const { return m_data.end(); }

private:
    int m_rows;
    int m_cols;
    vector m_data;
};

