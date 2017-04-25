#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include "Vector3.h"

using std::vector;

//function[oa_ctr, n_lenses] = array_setup(n_shells, pitch)
//% build a close - packed grid of circle centers.Start with a circle in the
//%   center, and then add successive shells around it
//

template<typename T>
vector<Point3<T>> ArraySetup(int n_shells, T pitch)
{
    //% determine lens optical axis centers shell by shell
    //% within each shell, the optical axis centers fall on the edges and
    //% vertices of a hexagon
    //% leave the z - values at 0, since nominal positions are in the x - y plane
    vector<Point3<T>> oa_ctr;

    // Create a center starting point
    oa_ctr.push_back(Point3<T>());

    //% first, populate the 6 vertices in every hexagonal shell
    for (int i_shell = 1; i_shell <= n_shells; ++i_shell)
    {
        for (int i_vertex = 0; i_vertex < 6; ++i_vertex)
        {
            oa_ctr.push_back(Point3<T>(
                static_cast<T>(i_shell * pitch * cos(i_vertex * M_PI / 3)),
                static_cast<T>(i_shell * pitch * sin(i_vertex * M_PI / 3)),
                static_cast<T>(0)));
        }
    }

    //% second, populate the edges in the hexagonal shell, based on the
    //% location of the 6 vertices in the shell
    for (int i_shell = 2; i_shell <= n_shells; ++i_shell)
    {
        //for i_edge = 1 : 6
        for (int i_edge = 1; i_edge <= 6; ++i_edge)
        {
            //% find the indices of the six vertices in the shell
            //% the first lens occupies index i = 1
            //% shell 1 : i = { 2, 3, 4, 5, 6, 7 }
            //% shell 2 : i = { 8, 9, 10, 11, 12, 13 }
            //% shell k : i = 6 * (k - 1) + {2, 3, 4, 5, 6, 7}
            int i_p1 = 6 * (i_shell - 1) + i_edge;
            auto p1 = oa_ctr[i_p1];
            auto i_p2 = (i_p1 + 1) % 6;
            auto p2 = oa_ctr[i_p2];
            auto v = (p2 - p1).Normalize();
            //% there are k edge points in the kth shell
            for (int i_edge_lens = 1; i_edge_lens < i_shell; ++i_edge_lens)
            {
                oa_ctr.push_back(Point3<T>(
                    static_cast<T>(p1.X() + i_edge_lens * pitch * v.X()),
                    static_cast<T>(p1.Y() + i_edge_lens * pitch * v.Y()),
                    static_cast<T>(0)));

            }
        }
    }

    return oa_ctr;
}
