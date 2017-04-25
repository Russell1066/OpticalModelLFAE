// OpticalModel LFAE.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Tests.h"
#include "NearField_R00.h"
#include "VectorMath.h"

using namespace VectorMath;

int main()
{
    assert(RunTests());
    
    NearField_R00();
    
    return 0;
}

