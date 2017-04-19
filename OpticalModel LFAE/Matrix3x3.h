#pragma once

#include "Vector3.h"

template <typename T>
struct Matrix3x3
{
    static_assert(std::is_arithmetic<T>::value, "Must be an arithmetic type (expecting float or double)");

    Matrix3x3()
    {
        memset(value, 0, sizeof(value));
    }

    template<size_t Size>
    Matrix3x3(T(&init)[Size])
    {
        CopyArray(init);
    }

    Matrix3x3(T a, T b, T c, T p, T q, T r, T u, T v, T w)
    {
        T init[] =
        {
            a, b, c,
            p, q, r,
            u, v, w
        };

        CopyArray(init);
    }

    template<size_t Size>
    void CopyArray(T(&init)[Size])
    {
        static_assert(Size == 9, "Must fill all elements with no leftovers");
        memcpy(value, init, sizeof(value));
    }

    T& A() { return value[0][0]; }
    T& B() { return value[0][1]; }
    T& C() { return value[0][2]; }

    T& P() { return value[1][0]; }
    T& Q() { return value[1][1]; }
    T& R() { return value[1][2]; }

    T& U() { return value[2][0]; }
    T& V() { return value[2][1]; }
    T& W() { return value[2][2]; }

    T const & A() const { return value[0][0]; }
    T const & B() const { return value[0][1]; }
    T const & C() const { return value[0][2]; }

    T const & P() const { return value[1][0]; }
    T const & Q() const { return value[1][1]; }
    T const & R() const { return value[1][2]; }

    T const & U() const { return value[2][0]; }
    T const & V() const { return value[2][1]; }
    T const & W() const { return value[2][2]; }

    Vector3<T> operator * (Vector3<T> const& rhs) const
    {
        return Vector3<T>(
            rhs.X() * A() + rhs.Y() * B() + rhs.Z() * C(),
            rhs.X() * P() + rhs.Y() * Q() + rhs.Z() * R(),
            rhs.X() * U() + rhs.Y() * V() + rhs.Z() * W()
            );
    }

    T value[3][3];
};
