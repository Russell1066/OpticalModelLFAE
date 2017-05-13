#pragma once

#include<string>
#include<vector>

#include "ConfigHelpers.h"
#include "DataType.h"

namespace FraunhoferFarField1D
{
    using json = nlohmann::json;
    using namespace jsonHelper;

    static const floatType Default_lambda = 1;
    static const floatType Default_b = 10 * Default_lambda;
    static const int Default_thetaDivisions = 100;
    static const floatType Default_thetaMax = M_PI_4;
    static const int Default_bDivisions = 1000;
    static const floatType Default_a = 1;
    static const floatType Default_radius = 3;

    struct Parameters
    {
        floatType lambda;           // wavelength
        floatType a;
        floatType b;                // bMin = -b
        floatType radius;
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

    inline void to_json(json& j, const Parameters& p) {
        j = json
        {
            { "lambda", p.lambda },
            { "b", p.b },
            { "thetaDivisions", p.thetaDivisions },
            { "thetaMax", p.thetaMax },
            { "bDivisions", p.bDivisions },
        };
    }

    inline void from_json(const json& j, Parameters& p)
    {
        p.lambda = GetValueOrDefault(j, "lambda", Default_lambda);
        p.a = GetValueOrDefault(j, "a", Default_a);
        p.b = GetValueOrDefault(j, "b", Default_b);
        p.radius = GetValueOrDefault(j, "radius", Default_radius);
        p.thetaDivisions = GetValueOrDefault(j, "thetaDivisions", Default_thetaDivisions);
        p.thetaMax = GetValueOrDefault(j, "thetaMax", Default_thetaMax);
        p.bDivisions = GetValueOrDefault(j, "bDivisions", Default_bDivisions);
    }

    inline void ValidateParameters(Parameters const& params)
    {
        std::vector<std::string> errors;
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

    inline Parameters LoadParameters(std::string const& paramFile, bool printParams = false)
    {
        auto p =  FromJson<FraunhoferFarField1D::Parameters>(paramFile);

        if (printParams)
        {
            printf("Calculating based on the following:\n%s", jsonHelper::GetJson(p).c_str());
        }

        return p;
    }
}

std::vector<floatType> ComputeFraunhoferFarField1DFlux(std::string const& paramFile = "");
