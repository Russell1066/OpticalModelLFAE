// OpticalModel LFAE.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Tests.h"
#include "NearField_R00.h"
#include "VectorMath.h"
#include "WriteToCSV.h"
#include "json.hpp"

using namespace VectorMath;
using json = nlohmann::json;

static const std::string file(".\\I_00.csv");
static const std::string parameters("Y:\\Work Area\\UCSB\\OpticalModelLFAE\\OpticalModel LFAE\\nearfield.json");

int main()
{
    assert(RunTests());

    auto t = NearField_R00(parameters);

    WriteToCSV(file, t);

    auto max = *t.begin();
    auto min = max;

    return 0;
}

