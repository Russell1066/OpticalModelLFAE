// OpticalModel LFAE.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "json.hpp"
#include "FraunhoferFarField1D.h"
#include "NearField_R00.h"
#include "Tests.h"
#include "VectorMath.h"
#include "WriteToCSV.h"

using namespace VectorMath;
using json = nlohmann::json;

static const std::string file(".\\I_00.csv");

int main(int argc, char*argv[])
{
    //assert(RunTests());

    if (argc < 2)
    {
        printf("Need the name of a configuration file\n");
        return -1;
    }

    //auto t = NearField_R00(argv[1]);

    //WriteToCSV(file, t);

    //auto max = *t.begin();
    //auto min = max;

    auto flux = ComputeFraunhoferFarField1DFlux(argv[1]);
    auto nPoints = flux.size() / 2;
    auto  beg = flux.begin();
    auto end = beg + nPoints;
    auto compare = flux.rbegin();
    dataType maxDiff = 0;
    for (; beg != end; ++beg, ++compare)
    {
        auto diff = abs(*beg - *compare);
        maxDiff = std::max(maxDiff, diff);
    }

    WriteToCSV(".\\ringingInTheNews.csv", flux);

    return 0;
}

