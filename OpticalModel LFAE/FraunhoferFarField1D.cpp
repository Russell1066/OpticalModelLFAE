//

#include "stdafx.h"

#include <assert.h>
#include <complex>
#include <vector>

#include "ConfigHelpers.h"
#include "DataType.h"
#include "VectorMath.h"
#include "WriteToCSV.h"

using json = nlohmann::json;
using namespace jsonHelper;

using std::string;
using std::vector;



template<typename resultType, typename rangeType, typename functionType>
resultType Integrate(rangeType start, rangeType end, functionType toIntegrate, int nSteps, rangeType &deltaRange)
{
    resultType result;

    deltaRange = (end - start) / (nSteps - 1);
    for (int i = 0; i < nSteps; ++i, start += deltaRange)
    {
        result += toIntegrate(start, deltaRange);
    }

    return result;
}

namespace FraunhoferFarField1D
{
    static const floatType Default_lambda = 1;
    static const floatType Default_b = 10 * Default_lambda;
    static const int Default_thetaDivisions = 100;
    static const floatType Default_thetaMax = M_PI_4;
    static const int Default_bDivisions = 1000;

    static const complexType _i(0, 1);

    struct Parameters
    {
        floatType lambda;           // wavelength
        floatType b;                // bMin = -b
        int bDivisions;             // deltaB = b / bDivisions
        floatType thetaMax;         // thetaMin = - thetaMax
        int thetaDivisions;         // deltaTheta = thetaMax * 2 / thetaDivisions

        Parameters() :
            lambda(Default_lambda),
            b(Default_b),
            bDivisions(Default_bDivisions),
            thetaMax(Default_thetaMax),
            thetaDivisions(Default_thetaDivisions)
        {

        }
    };

    void to_json(json& j, const Parameters& p) {
        j = json
        {
            { "lambda", p.lambda },
            { "b", p.b },
            { "thetaDivisions", p.thetaDivisions },
            { "thetaMax", p.thetaMax },
            { "bDivisions", p.bDivisions },
        };
    }

    void from_json(const json& j, Parameters& p)
    {
        p.lambda = GetValueOrDefault(j, "lambda", Default_lambda);
        p.b = GetValueOrDefault(j, "b", Default_b);
        p.thetaDivisions = GetValueOrDefault(j, "thetaDivisions", Default_thetaDivisions);
        p.thetaMax = GetValueOrDefault(j, "thetaMax", Default_thetaMax);
        p.bDivisions = GetValueOrDefault(j, "bDivisions", Default_bDivisions);
    }

    void ValidateParameters(Parameters const& params)
    {
        vector<string> errors;
        assert(params.thetaDivisions > 0);
        if (params.thetaDivisions <= 0)
        {
            errors.push_back("Must have more than 0 output points - pick something reasonable - bad thetaDivisions");
        }

        assert(params.lambda > 0);
        if (params.lambda <= 0)
        {
            errors.push_back("Wavelengths are positive - bad lambda");
        }

        assert(params.b > 0);
        if (params.b <= 0)
        {
            errors.push_back("range of b must be positive - bad b");
        }

        assert(params.bDivisions > 0);
        if (params.bDivisions <= 0)
        {
            errors.push_back("number of divisions of b must be positive - bad bDivisions");
        }

        if (errors.size() > 0)
        {
            throw errors;
        }
    }

    class FluxCalculator
    {
    public:
        FluxCalculator(Parameters const& params)
        {
            minB = -params.b / 2;
            maxB = -minB;
            bDivisions = params.bDivisions;
            deltaB = params.b / (bDivisions - 1);

            k = 2 * M_PI / params.lambda;
        }

        floatType Compute(floatType theta)
        {
            auto sinTheta = sin(theta);

            floatType delta = 0;
            complexType integral = Integrate<complexType>(minB, maxB, [&](floatType y, floatType deltaY) {
                return exp(_i * k * y * sinTheta);
            }, bDivisions, delta);

            integral *= delta;

            return std::norm(integral);
        }

    private:
        floatType minB;
        floatType maxB;
        floatType deltaB;
        int bDivisions;

        floatType k;
    };

    vector<floatType> ComputeFlux(Parameters const& params)
    {
        vector<floatType> retv;

        ValidateParameters(params);

        FluxCalculator fluxCalculator(params);

        retv.reserve(params.thetaDivisions);

        floatType thetaMax = params.thetaMax;
        floatType thetaMin = -thetaMax;
        int maxIndex = params.thetaDivisions;
        floatType deltaTheta = (thetaMax - thetaMin) / (maxIndex - 1);

        floatType theta = thetaMin;
        for (int thetaIndex = 0; thetaIndex < maxIndex; ++thetaIndex, theta += deltaTheta)
        {
            retv.push_back(fluxCalculator.Compute(theta));
        }

        return retv;
    }
}

vector<floatType> ComputeFraunhoferFarField1DFlux(std::string const& paramFile)
{
    auto p = FromJson<FraunhoferFarField1D::Parameters>(paramFile);

    printf("Calculating based on the following:\n%s", jsonHelper::GetJson(p).c_str());

    return (ComputeFlux(p));
}
