// OpticalModel LFAE.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "FraunhoferFarField1D.h"
#include "NearField_R00.h"
#include "Tests.h"
#include "WriteToCSV.h"

using std::string;

int main(int argc, char*argv[])
{
    assert(RunTests());

    string parameterFile;
    string outputFile = ".\\output.csv";

    if (argc > 1)
    {
        parameterFile = argv[1];
    }

    if (argc > 2)
    {
        outputFile = argv[2];
    }

    auto flux = ComputeFraunhoferFarField1DFlux(parameterFile);

    try
    {
        WriteToCSV(outputFile, flux);
    }
    catch (...)
    {
        printf("Output file '%s' not written", outputFile.c_str());
    }

    return 0;
}

