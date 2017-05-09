//

#include "stdafx.h"

#include <assert.h>
#include <complex>
#include <vector>

#include "DataType.h"
#include "Integrals.h"
#include "FraunhoferFarField1D.h"

using std::vector;

using namespace Integrals;

namespace FraunhoferFarField1D
{
    static const complexType _i = { 0, 1 };

    class FluxCalculator
    {
    public:
        FluxCalculator(Parameters const& params)
        {
            minB = -params.b / 2;
            maxB = -minB;
            bDivisions = params.bDivisions;

            k = 2 * M_PI / params.lambda;
        }

        // Given a set of parameters
        // Calculate the flux at the range of angles given
        static vector<floatType> ComputeFlux(Parameters const& params)
        {
            vector<floatType> retv;

            ValidateParameters(params);

            FluxCalculator fluxCalculator(params);

            retv.resize(params.thetaDivisions);

            floatType thetaMax = params.thetaMax;
            floatType thetaMin = -thetaMax;

            for_closed_range(thetaMin, thetaMax, [&retv, fluxCalculator](auto i, auto theta, auto delta) {
                retv[i] = fluxCalculator.Compute(theta);
            }, params.thetaDivisions);

            return retv;
        }

    private:
        floatType Compute(floatType theta) const
        {
            auto sinTheta = sin(theta);

            auto integral = Integrate<complexType>(minB, maxB, [&](floatType b) {
                return exp(_i * k * b * sinTheta);
            }, bDivisions);

            return std::norm(integral);
        }

    private:
        floatType minB;
        floatType maxB;
        int bDivisions;

        floatType k;
    };

}

using namespace FraunhoferFarField1D;

vector<floatType> ComputeFraunhoferFarField1DFlux(std::string const& paramFile)
{
    auto p = LoadParameters(paramFile, true);

    return (FluxCalculator::ComputeFlux(p));
}
