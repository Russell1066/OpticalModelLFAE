#pragma once

#include "Vector3.h"

namespace VectorMath
{
    template<typename T>
    bool ApproximatelyEqual(T lhs, T rhs)
    {
        T temp = lhs - rhs;

        return (temp >= 0) ? temp < Vector3<T>::LAMBDA : temp > -Vector3<T>::LAMBDA;
    }

    template<typename T>
    bool ApproximatelyZero(T value)
    {
        return ApproximatelyEqual(value, 0.f);
    }

    template<typename T>
    bool ApproximatelyEqual(Vector3<T> const& lhs, Vector3<T> const& rhs)
    {
        return ApproximatelyEqual(lhs.X(), rhs.X()) &&
            ApproximatelyEqual(lhs.Y(), rhs.Y()) &&
            ApproximatelyEqual(lhs.Z(), rhs.Z());
    }

    template <typename T>
    Vector3<T> CrossProduct(Vector3<T> const& u, Vector3<T> const& v)
    {
        Vector3<T> retv;
        retv.X() = u.Y() * v.Z() - v.Y() * u.Z();
        retv.Y() = -(u.X() * v.Z() - v.X() * u.Z());
        retv.Z() = u.X() * v.Y() - v.X() * u.Y();

        return retv;
    }

    template <typename T>
    T DotProduct(Vector3<T> const& u, Vector3<T> const& v)
    {
        return u.X() * v.X() + u.Y() * v.Y() + u.Z() * v.Z();
    }

    // calculates the distance D from a point Q to a plane that contains the
    // point P, where the distance D is along the plane normal vector N    
    template <typename T>
    T PointPlaneNormalDistance(Point3<T> const& Q, Vector3<T> N, Point3<T> const& P)
    {
        auto norm = N.Norm();
        if (ApproximatelyZero(norm))
        {
            return 0;
        }

        N = N / norm;

        return DotProduct(n, Q) - DotProduct(n, P);
    }

    // calculates the distance D from a point Q to a plane that contains the
    // point P, where the destination plane normal vector is N, and where the
    // distance D is along the arbitrary vector V

    // Q point of origin
    // N destination plane normal vector
    // P any point on the destination plane
    // V direction vector from point Q to the destination plane
    template<typename T>
    T PointPlaneObliqueDistance(Point3<T> const& Q, Vector3<T> N, Point3<T> const& P, Vector3<T> V)
    {
        auto normN = N.Norm();
        auto normV = V.Norm();
        if (ApproximatelyZero(normN) || ApproximatelyZero(normV))
        {
            throw "'CheckData:InputError', ' an input vector is degenerate'";
        }

        N = N / normN;
        V = V / normV;

        // if the direction vector V is orthogonal to the plane normal vector,
        // then no distance can be calculated
        auto dotNV = DotProduct(N, V);
        if (ApproximatelyZero(dotNV))
        {
            throw "'CheckData:InputError', ' V is orthogonal to N'";
        }

        return (DotProduct(P, N) - DotProduct(N, Q)) / dotNV;
    }

    template<typename T>
    Vector3<T> RotatePoint3d(Point3<T> const& r_center, T r_theta, Vector3<T> r_axis, Point3<T> point)
    {
        if (ApproximatelyZero(r_theta) || ApproximatelyZero(r_axis.Norm()))
        {
            return point;
        }

        r_axis.Normalize();
        auto cosTheta = cos(r_theta);
        auto oneMinusCosTheta = 1 - cosTheta;
        auto sinTheta = sin(r_theta);

        T b[3][3] = {
            static_cast<T>(cosTheta + r_axis.X() * r_axis.X() * oneMinusCosTheta),
            static_cast<T>(-r_axis.Z() * sinTheta + r_axis.X() * r_axis.Y() * oneMinusCosTheta),
            static_cast<T>(r_axis.Y() * sinTheta + r_axis.X() * r_axis.Z() * oneMinusCosTheta),

            static_cast<T>(r_axis.Z() * sinTheta + r_axis.X() * r_axis.Y() * oneMinusCosTheta),
            static_cast<T>(cosTheta + r_axis.Y() * r_axis.Y() * oneMinusCosTheta),
            static_cast<T>(-r_axis.X() * sinTheta + r_axis.Y() * r_axis.Z() * oneMinusCosTheta),

            static_cast<T>(-r_axis.Y() * sinTheta + r_axis.X() * r_axis.Z() * oneMinusCosTheta),
            static_cast<T>(r_axis.X() * sinTheta + r_axis.Y() * r_axis.Z() * oneMinusCosTheta),
            static_cast<T>(cosTheta + r_axis.Z() * r_axis.Z() * oneMinusCosTheta)
        };

        point = point - r_center;

        // matrix multiply here
        Vector3<T> result(
            point.X() * b[0][0] + point.Y() * b[0][1] + point.Z() * b[0][2],
            point.X() * b[1][0] + point.Y() * b[1][1] + point.Z() * b[1][2],
            point.X() * b[2][0] + point.Y() * b[2][1] + point.Z() * b[2][2]
        );

        return result + r_center;
    }
}