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

int main(int argc, char*argv[])
{
    assert(RunTests());

    if (argc < 2)
    {
        printf("Need the name of a configuration file\n");
        return -1;
    }

    auto t = NearField_R00(argv[1]);

    WriteToCSV(file, t);

    auto max = *t.begin();
    auto min = max;

    return 0;
}

