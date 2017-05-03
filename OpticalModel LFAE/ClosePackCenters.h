#pragma once

#include <vector>
#include "Vector3.h"

// Remove once we have established that we have the right algorithm
std::vector<FloatPoint> ClosePackCentersFloat(int nshells, double outer_rad);

//function[discr_ctr, discr_rad, ndiscr_tot] = ...
//close_pack_centers(nshells, outer_rad)
//% build a close - packed grid of circle centers, within a circumscribing
//%   circle centered at the origin.Start with a circle in the center, and
//%   determine a radius such that nshells circles fit along one radius of
//%   the circumscribing circle
//
//% build discretization centers by rows in the circular aperture
//% leave the z - values at 0, since nominal positions are in the x - y plane
template<typename T>
std::vector<Point3<T>> ClosePackCenters(int nshells, T outer_rad, T& discr_rad)
{
    discr_rad = outer_rad / (2 * nshells + 1);
    int nrow = static_cast<int>(round(outer_rad / discr_rad + 1));
    std::vector<Point3<T>> discr_ctr;

    // the center is at the origin
    discr_ctr.push_back(Point3<T>());

    // Create the circles that lie on the x-axis
    for (int icol = 1; icol <= nshells; ++icol)
    {
        auto xcenter = static_cast<T>(2 * icol * discr_rad);
        discr_ctr.push_back(Point3<T>(xcenter, 0, 0));
        discr_ctr.push_back(Point3<T>(-xcenter, 0, 0));
    }

    auto rowlimit = static_cast<int>(floor(2 * nshells / sqrt(3)));
    auto sqrt3 = sqrt(3);
    auto radiusLimit = 0.999*discr_rad;

    for (int irow = 1; irow <= rowlimit; ++irow)
    {
        // Even numbered rows have a center along the y-axis
        if ((irow & 1) == 0)
        {
            auto ycenter = static_cast<T> (irow*discr_rad*sqrt(3));

            discr_ctr.push_back(Point3<T>(0, ycenter, 0));
            discr_ctr.push_back(Point3<T>(0, -ycenter, 0));
        }

        auto yctr = static_cast<T>(irow * discr_rad * sqrt3);

        for (int icol = 1; icol <= nshells; ++icol)
        {
            auto xctr = static_cast<T>((2 * icol - (irow & 1)) * discr_rad);

            auto radius = sqrt(xctr * xctr + yctr * yctr);
            if (radius > outer_rad - radiusLimit)
            {
                // Once we are outside the limit, stop looking and go to the next row
                break;
            }

            discr_ctr.push_back(Point3<T>(xctr, yctr, 0));
            discr_ctr.push_back(Point3<T>(-xctr, yctr, 0));
            discr_ctr.push_back(Point3<T>(xctr, -yctr, 0));
            discr_ctr.push_back(Point3<T>(-xctr, -yctr, 0));
        }
    }

    return discr_ctr;
}