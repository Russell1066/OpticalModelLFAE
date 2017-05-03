#include "stdafx.h"

#define _USE_MATH_DEFINES

#include <algorithm>
#include <assert.h>
#include <complex>
#include <math.h>
#include <thread>
#include <vector>

#include "ArraySetup.h"
#include "Array2D.h"
#include "ClosePackCenters.h"
#include "NearField_R00.h"
#include "VectorMath.h"
#include "WriteToCSV.h"

using std::vector;
using namespace VectorMath;


template<typename T> void NormalizeVectors(std::vector<Point3<T>> &v);
void RandomizeCenters(pointVector &v, const dataType &rand_polarization);

void WriteVector(std::string const& name, pointVector &oa_center);

template <typename dataType>
Array2D<Point3<dataType>> GetTarget(int side, dataType extent, dataType distance);

Point3<dataType>
mean(pointVector const& v);

const auto i = complexType(0, 1);
const auto _2 = complexType(2);
const auto _1 = complexType(1);
const Vector3<dataType> z(0, 0, 1);

class NearField
{
public:
    //% Declared System Configuration Details
    //%= ==================================== =
    //%   Dlens = diameter of each lens, meters
    dataType Dlens = 0.5;

    //%   lens_pitch = lens pitch, meters; center - to - center distance from the
    //%   center lens to a lens in the first 'shell'
    dataType lens_pitch = 0.5;

    //
    //%   n_lens_shells = number of lens 'shells' on one radius of the array, not
    //%   including the center lens
    int n_lens_shells = 3;

    //
    //%   lambda = nominal emitter wavelength, meters
    dataType lambda = static_cast<dataType>(1.064e-6);

    //%   Demitter = fiber tip diameter of all emitters, meters
    dataType Demitter = static_cast<dataType>(12.5e-6);

    //%   NAemitter = Numerical Aperture of all fiber tips
    dataType NAemitter = static_cast<dataType>(0.22);

    //%   FOV = optical(angular) field of view for all lenses, radians
    //%         From Mission Parameters, e.g., produce a 1 m beam at 100 km
    dataType FOV = static_cast<dataType>(1e-5);

    //%   n_medium = index of refraction of the medium(1 in vacuum of space)
    dataType n_medium = 1;

    //%   target_surf_dist = distance of the target surface from the nominal
    //%   emitter array center, meters
    dataType target_surf_dist = 1000;

    //%   ndiscr = number of 'shells' on one radius of a lens element, not
    //%   including the center point
    int n_discr_shells = 5;

    int npts = 100;
    dataType gmax = 3;

    int m_threadCount = static_cast<int>(std::thread::hardware_concurrency());

    //dataType gmin = -3;

public:
    Array2D<dataType> R00()
    {
        //% Near Field Optical Model for an array of laser fiber amplifier emitters
        //% An array of N emitters is modeled by prescribing the(nominal) location
        //% of each emitter, and its associate lens diameter and optical axis
        //% pointing vector.
        //% The near field intensity is calculated on some target surface, e.g., on
        //% a perpendicular(normal) plane located at some distance in front of the
        //% array.
        //% The intensity is calculated by summing complex phases from contributions
        //% of discretized points on each lens; the intensity is calculated at
        //% selected locations on the target surface.
        //
        //% Initial Version 03 / 12 / 2017 --gbh
        //% Modified 03 / 28 / 2017 --added aperture to U(x, y) --gbh
        //% Modified 04 / 13 / 2017 --changed lens size determination --gbh
        //

        dataType Rlens = Dlens / 2;

        //
        //%   lens_pitch must be greater than or equal to Dlens
        assert(lens_pitch >= Dlens);

        dataType array_radius = n_lens_shells*lens_pitch;
        k = static_cast<dataType>(2 * M_PI / lambda);

        //
        //
        //
        //
        //% Declared Optical Properties
        //%= ========================== =
        //%   i_lens = lens index, 1, 2, ... n_lenses
        //%   oa_center(i_lens) = nominal optical center positions(x, y, z), meters
        //%                       for each lens in the array
        //%   n_lenses = total number of lenses in the array, calculated from the
        //%                       number of desired emitter shells and the lens
        //%                       pitch previously declared

        auto oa_center = ArraySetup<dataType>(n_lens_shells, lens_pitch);
        WriteVector("oa_center", oa_center);

        int n_lenses = static_cast<int>(oa_center.size());

        //%   oa_vector(i_lens) = nominal optical axis pointing vectors, meters
        auto oa_vector = std::vector<Point3<dataType>>(n_lenses, z);
        WriteVector("oa_vector", oa_vector);

        // Since the vector was initialized as normalized, this is unnecessary
        //NormalizeVectors(oa_vector);

        //
        //% Build a nominal lens discretization grid
        //%= ========================================
        //
        //%   the grid is centered at the origin, using close - packed circular
        //%   discrete elements within the lens aperture
        //[discr_ctr, discr_rad, n_lens_pts] = ...
        dataType discr_rad;
        auto discr_ctr = ClosePackCenters<dataType>(n_discr_shells, Rlens, discr_rad);
        WriteVector("ClosePackCenters", discr_ctr);
        int n_lens_pts = static_cast<int>(discr_ctr.size());

        //%   the total number of discretized lens elements in the entire array is
        //%   determined from the numer of points within each lens times the number
        //%   of lenses in the overall array
        int n_total_points = n_lenses * n_lens_pts;
        //
        //%   phi(p) = phase angle for each lens discretization point, radians
        //phi = zeros(n_total_points, 1);
        auto phi = vector<dataType>(n_total_points, 1);
        //
        //%   use the nominal grid as a template to create the actual emitter
        //%   aperture discretizations, by translating and rotating the nominal grid
        //%   to each optical center and optical vector
        auto lens_pts = Array2D<pointType>(n_lenses, n_lens_pts);

        //for i_lens = 1:n_lenses
        for (int i_lens = 0; i_lens < n_lenses; ++i_lens)
        {
            //% give the pattern a whirl, to 'randomize' polarization ?
            //    rand_polarization = pi*rand(1, 1);
            dataType rand_polarization = static_cast<dataType>(M_PI * rand() / RAND_MAX);
            //RandomizeCenters(discr_ctr, rand_polarization);

            //% first, align the current aperture to the optical vector
            //    % rotation axis is the cross product of the optical axis with the
            //    % z - axis, since the template was built in the x - y plane
            auto raxis = CrossProduct(oa_vector[i_lens], z);

            //       % product with the z-axis will be a null vector, with norm zero;
            //       % otherwise, the cross-product vector will have some length.
            //       norm_raxis = normV (raxis);
            auto norm_raxis = raxis.Norm();
            //       if (norm_raxis > 0)
            if (norm_raxis > 0)
            {
                //           % force rotation axis to length 1
                //           raxis = raxis./normV (raxis);

                //           % oa_vector was normalized at definition
                //           dot_oa_z = dotProductV1V2 (oa_vector(i_lens,:), [0 0 1]);
                //           rtheta = acos (dot_oa_z);

                //           % loop through the discretized lens points to rotate and
                //           % translate into the lens position
                //           for i_pt = 1:n_lens_pts
                //               % rotation center is the origin
                //               lens_pts(n_lens_pts*(i_lens - 1) + i_pt,:) = ...
                //                RotatePoint3d ([0 0 0], rtheta, raxis, discr_ctr(i_pt,:));
                //               % translate the rotated point out to the lens position
                //               lens_pts(n_lens_pts*(i_lens - 1) + i_pt,:) = ...
                //                       lens_pts(n_lens_pts*(i_lens - 1) + i_pt,:) + ...
                //                       oa_center(i_lens,:);
                //           end
            }
            else
            {
                auto lensPtIter = lens_pts[i_lens].begin();
                for (auto const& centerPt : discr_ctr)
                {
                    *lensPtIter = centerPt + oa_center[i_lens];
                    ++lensPtIter;
                }
            }
        }
        //% Define the Target Surface
        //%= ======================== =
        //% setup a 'footprint' in the x - y plane around the center of the array,
        //% where the footprint points will be projected directly upward onto the
        //% target surface.
        //% e.g., the footprint can be a square grid, with equal spacing in x and y,
        //% centered around the origin, which is also the nominal center of the
        //% emitter array
        //npts = 100;
        //gmin = -3;
        //gmax = -gmin;
        //deltag = (gmax - gmin) / (npts - 1);
        //
        //% calculate footprint grid locations
        //gv = (gmin:deltag:gmax)';
        //[X, Y] = meshgrid(gv, gv);
        auto target = GetTarget<dataType>(npts, gmax, target_surf_dist);

        //
        //% calculate locations on the target surface above the footprint
        //% e.g., target surface is a horizontal plane 100 km high; store the
        //% plane using an appropriate plane equation over the meshgrid
        //Z = 0 * X + 0 * Y + target_surf_dist;
        //
        //% Calculate Beam Intensity in the Target Plane
        //%= ============================================
        //% Seek to determine the beam intensity at each point in the target surface
        //% by calculating the sum of complex phases for each discretized lens point.
        //% The complex phase is determined by the distance from the lens point
        //% to its projection onto the lens reference plane.
        //
        //% U(x, y, z) = SUM(i_pt from 1 to n_total_points)
        //% {
        //    [((e ^ (i k a sin(theta(i_pt))) - 1)] / (i k a sin(theta(i_pt)))] *
        //    %[(e ^ (i(k t(i_pt) + phi(i_pt)))]
        //}
        //% theta(i_pt) = angle between the optical axis and a vector from(x, y, z) to
        //%               the discretized lens point
        //% t(i_pt) = projection distance from the discretized lens point to the
        //%           array reference plane along the vector from the lens point to
        //%           the point on the target surface
        //% phi(i_pt) = a phase perturbation term; will include fixed misalignment
        //%             perturbations, transient misalignmet perturbations, and
        //%             controlled changes to emitter phase for beam pointing and
        //%             shaping goals
        //% a = discretization radius, determined by the lens discretization setup
        //% k = 2 * pi / lambda;
        //
        //% The intensity is I(x, y, z) = [magnitude(U(x, y, z))] ^ 2
        //
        //% need to sum contributions from each emitter at each location on the
        //% target surface
        //% initialize complex phase storage array to zeros
        //U = zeros(npts, npts);
        //U = complex(U, 0);

        // U can probably be eliminated 
        // by calculating U for every point on the target, the value of I can be calculated at that point
        // this eliminates the storage requirement for U entirely - since it isn't graphed, there's no later use of it
        Array2D<dataType> I(npts, npts);
        //% initialize(real - valued) intensity storage array to zeros
        //I = zeros(npts, npts);
        //
        //for i_x = 1:npts
        //for i_y = 1 : npts

        //% loop through points on the target surface

        //% define Qi to be the current point on the target surface
        //Qi = [X(i_x, i_y) Y(i_x, i_y) Z(i_x, i_y)];

        //% the reference plane for the array is anchored at a point that is the
        //% average location of optical centers in the array. **Calculate inside
        //% the loop to allow for changing optical center positions**
        // # note, not done because oa centers are not currently changing
        //refplane_anchor = mean(oa_center, 1);
        auto refplane_anchor = mean(oa_center);

        if (m_threadCount <= 1)
        {
            for (int i = 0; i < static_cast<int>(target.size()); ++i)
            {
                *(I.begin() + i) = ShineOnTargetPoint(target, i, refplane_anchor, n_lenses, oa_vector, n_lens_pts, lens_pts, discr_rad, phi);
            };
        }
        else
        {
            int qi = 0;
            int stride = static_cast<int>((target.size() + m_threadCount - 1) / m_threadCount);
            vector<std::thread> threads;
            for (int proc = 0; proc < m_threadCount; ++proc)
            {
                threads.push_back(std::thread([&, qi, stride]() {
                    for (int i = 0; i < stride; ++i)
                    {
                        *(I.begin() + qi + i) = ShineOnTargetPoint(target, qi + i, refplane_anchor, n_lenses, oa_vector, n_lens_pts, lens_pts, discr_rad, phi);
                    }
                }));
                qi += stride;
                stride = std::min<int>(stride, target.size() - qi);
            }

            for (auto& thread : threads)
            {
                thread.join();
            }
        }

        //% theta(i) = angle between the optical axis and a vector
        //% from the point Qi = (x, y, z) on the target surface to the
        //% discretized lens point Pi
        //% oa_vector was normalized on definition
        //dot_QiPi_oa = dotProductV1V2(oa_vector(i_lens, :), QiPi);
        //theta_i = acos(dot_QiPi_oa);
        //
        //% t = projection distance from the discretized lens point Pi to
        //% the array reference plane along the vector from the lens
        //% point P) to the point Qi on the target surface
        //t_i = point_plane_oblique_distance(Pi, refplane_N, ...
        //    refplane_anchor, QiPi);
        //
        //% U(i_pt) = { [((e ^ (i k a sin(theta(i_pt))) - 1)] /
        //    % (i k a sin(theta(i_pt))) }*
        //    %[(e ^ (i(k t(i_pt) + phi(i_pt)))]
        //        U(i_x, i_y) = U(i_x, i_y) + ...
        //        ((exp(1i*k * 2 * discr_rad*sin(theta_i)) - 1) / ...
        //        (1i*k * 2 * discr_rad*sin(theta_i)))* ...
        //            (exp(1i*(k*t_i + phi(i_pt))));
        //end
        //end
        //% intensity is I(i_pt) = [magnitude(U(i_pt))] ^ 2
        //I(i_x, i_y) = (abs(U(i_x, i_y))) ^ 2;
        //end
        //end
        //
        //% calculate intensity attenuation compared to the highest intensity on the
        //% target surface
        //dbI = 10 * log10(I / max(I(:)));
        //
        //% plot 3D oblique view of intensity attenuation
        //min_dbI = min(dbI(:));

        return I;
    }

    dataType ShineOnTargetPoint(Array2D<pointType>& target, int offset, Point3<double> &refplane_anchor, int n_lenses, pointVector &oa_vector, int n_lens_pts, Array2D<pointType> &lens_pts, dataType &discr_rad, std::vector<dataType> &phi)
    {
        auto Qi = *(target.begin() + offset);
        auto refplane_N = Qi - refplane_anchor;
        if (ApproximatelyZero(refplane_N.Norm()))
        {
            throw "'CheckData:InputError', ' refplane_N is degenerate'";
        }
        refplane_N.Normalize();

        complexType U;

        for (int lens = 0; lens < n_lenses; ++lens)
        {
            auto oa_lens = oa_vector[lens];
            //% the sum of complex phases U(xpi, ypi, zpi) at every point in the target
            //% surface will be over the number of lenses in the array, and then over
            //% the number of discretization points in each lens
            for (int i_pt = 0; i_pt < n_lens_pts; ++i_pt)
            {
                auto Pi = lens_pts[lens][i_pt];
                //% establish a vector QiPi from the point Qi = (x, y, z) on the
                //% target surface to the discretized lens point Pi
                auto QiPi = (Pi - Qi).Normalize();
                auto dot_QiPi_oa = DotProduct(oa_lens, QiPi);
                auto theta_i = acos(dot_QiPi_oa);
                auto t_i = PointPlaneObliqueDistance(Pi, refplane_N, refplane_anchor, QiPi);
                auto sintheta_i = sin(theta_i);
                U += ((exp(i * k * _2 * discr_rad * sintheta_i) - _1) / (i * k * _2 * discr_rad * sintheta_i))* (exp(i * (k*t_i + phi[i_pt])));
            }
        }

        return std::norm(U);
    }

private:
    dataType k;
};

void WriteVector(std::string const& name, pointVector &oa_center)
{
    static const std::string dir("C:\\Users\\Lori\\Documents\\");
    WriteToCSV(dir + name + ".csv", oa_center);
}

void RandomizeCenters(pointVector &v, const dataType &rand_polarization)
{
    dataType cos_theta = static_cast<dataType>(cos(rand_polarization));
    dataType sin_theta = static_cast<dataType>(sin(rand_polarization));
    for (pointVector::value_type& value : v)
    {
        auto x = cos_theta * value.X() + sin_theta * value.Y();
        auto y = sin_theta * value.X() + cos_theta * value.Y();

        value.X() = x;
        value.Y() = y;
    }
}

template<typename T>
void NormalizeVectors(std::vector<Point3<T>> &v)
{
    for each (Point3<T>& lens in v)
    {
        lens.Normalize();
    }
}

template <typename dataType>
Array2D<Point3<dataType>> GetTarget(int side, dataType extent, dataType distance)
{
    assert(side > 0);
    assert(extent > 0);

    Array2D<Point3<dataType>> data(side, side);

    auto min = -extent;
    auto max = extent;
    auto range = max - min;
    auto increment = range / (side - 1);

    auto iter = data.begin();
    for (int y = 0; y < side; ++y)
    {
        dataType yVal = y * increment + min;
        for (int x = 0; x < side; ++x, ++iter)
        {
            iter->X() = x * increment + min;
            iter->Y() = yVal;
            iter->Z() = distance;
        }
    }

    return data;
}

Point3<dataType>
mean(pointVector const& v)
{
    Point3<dataType> retv;

    assert(v.size() > 0);

    for (auto const& p : v)
    {
        retv = retv + p;
    }

    return retv / static_cast<dataType>(v.size());
}

Array2D<dataType> NearField_R00()
{
    NearField n;
    n.Dlens = 0.5;
    n.lens_pitch = 0.5;
    n.n_lens_shells = 0;
    n.lambda = 1.064e-6;
    n.Demitter = 12.5e-6;
    n.NAemitter = 0.22;
    n.FOV = 1e-5;
    n.target_surf_dist = 1000;
    n.n_discr_shells = 20;
    n.npts = 1000;
    n.gmax = 1;
    n.m_threadCount = 8;

    return n.R00();
}

