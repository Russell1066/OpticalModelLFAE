#pragma once
#include <type_traits>

template <typename T>
struct Vector3
{
    static_assert(std::is_arithmetic<T>::value, "Must be an arithmetic type (expecting float or double)");
    static const T LAMBDA; // differences less than lambda are considered zero
    typedef T value_type;

    // empty constructor
    Vector3()
    {
        value[0] = 0;
        value[1] = 0;
        value[2] = 0;
    }

    // specified constructor
    Vector3(T x, T y, T z)
    {
        X() = x;
        Y() = y;
        Z() = z;
    }

    // accessors
    T& X() { return value[0]; }
    T& Y() { return value[1]; }
    T& Z() { return value[2]; }

    // const accessors
    T const& X() const { return value[0]; }
    T const& Y() const { return value[1]; }
    T const& Z() const { return value[2]; }

    Vector3 operator + (Vector3 rhs) const
    {
        return Vector3(X() + rhs.X(), Y() + rhs.Y(), Z() + rhs.Z());
    }

    Vector3 operator - (Vector3 rhs) const
    {
        return Vector3(X() - rhs.X(), Y() - rhs.Y(), Z() - rhs.Z());
    }

    bool operator == (Vector3 rhs) const
    {
        bool r = true;
        r = r && fabs(X() - rhs.X()) < LAMBDA;
        r = r && fabs(Y() - rhs.Y()) < LAMBDA;
        r = r && fabs(Z() - rhs.Z()) < LAMBDA;

        return r;
    }

    Vector3<T> operator / (T divisor)
    {
        //assert(!ApproximatelyEqual(divisor, 0));

        return Vector3(X() / divisor, Y() / divisor, Z() / divisor);
    }

    T Norm() const
    {
        return static_cast<T>(sqrt(X() * X() + Y() * Y() + Z() * Z()));
    }

    Vector3<T>& Normalize(bool willThrow = true)
    {
        auto norm = Norm();
        if (willThrow || norm != 0)
        {
            *this = *this / Norm();
        }
        return *this;
    }

    T value[3];
};

template<typename T>
const T Vector3<T>::LAMBDA = static_cast<T>(0.00001);

template<typename T>
using Point3 = Vector3<T>;

typedef Vector3<float> FloatVector;
typedef Vector3<double> DoubleVector;
typedef Point3<float> FloatPoint;
typedef Point3<double> DoublePoint;
