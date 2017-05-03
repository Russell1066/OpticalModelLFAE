// OpticalModel LFAE.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Tests.h"
#include "NearField_R00.h"
#include "VectorMath.h"
#include "WriteToCSV.h"

using namespace VectorMath;

static const std::string file("C:\\Users\\Lori\\Documents\\I_00.csv");

int main()
{
    //assert(RunTests());

    auto t = NearField_R00();

    WriteToCSV(file, t);

    auto max = *t.begin();
    auto min = max;

    return 0;
}

