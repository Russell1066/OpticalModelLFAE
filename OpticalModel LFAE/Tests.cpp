#include "stdafx.h"

#include "VectorMath.h"
#include "tests.h"

using namespace VectorMath;

bool RunTests()
{
    assert(TestCrossProduct());
    assert(TestDotProduct());
    assert(TestNorm());
    assert(TestRotate3d());

    return true;
}

bool TestCrossProduct()
{
    FloatVector x(1, 0, 0);
    FloatVector y(0, 1, 0);
    FloatVector z(0, 0, 1);

    bool passed = true;
    passed = passed && ApproximatelyEqual(CrossProduct(x, y), FloatVector(0, 0, 1));
    passed = passed && ApproximatelyEqual(CrossProduct(y, x), FloatVector(0, 0, -1));

    passed = passed && ApproximatelyEqual(CrossProduct(x, z), FloatVector(0, -1, 0));
    passed = passed && ApproximatelyEqual(CrossProduct(z, x), FloatVector(0, 1, 0));

    passed = passed && ApproximatelyEqual(CrossProduct(y, z), FloatVector(1, 0, 0));
    passed = passed && ApproximatelyEqual(CrossProduct(z, y), FloatVector(-1, 0, 0));

    passed = passed && ApproximatelyEqual(CrossProduct(y, y), FloatVector(0, 0, 0));
    passed = passed && ApproximatelyEqual(CrossProduct(x, x), FloatVector(0, 0, 0));
    passed = passed && ApproximatelyEqual(CrossProduct(z, z), FloatVector(0, 0, 0));

    return passed;
}

bool TestDotProduct()
{
    bool passed = true;

    passed = passed && ApproximatelyEqual(DotProduct(FloatVector(1, 1, 1), FloatVector(1, 1, 1)), 3.f);
    passed = passed && ApproximatelyEqual(DotProduct(FloatVector(1, 0, 1), FloatVector(0, 1, 0)), 0.f);
    passed = passed && ApproximatelyEqual(DotProduct(FloatVector(1, 0, 0), FloatVector(1, 1, 1)), 1.f);
    passed = passed && ApproximatelyEqual(DotProduct(FloatVector(0, 1, 0), FloatVector(1, 1, 1)), 1.f);
    passed = passed && ApproximatelyEqual(DotProduct(FloatVector(0, 0, 1), FloatVector(1, 1, 1)), 1.f);

    return passed;
}

bool TestNorm()
{
    FloatVector x(1, 0, 0);
    FloatVector y(0, 1, 0);
    FloatVector z(0, 0, 1);

    bool passed = true;
    passed = passed && ApproximatelyEqual(x.Norm(), 1.f);
    passed = passed && ApproximatelyEqual(y.Norm(), 1.f);
    passed = passed && ApproximatelyEqual(z.Norm(), 1.f);

    return passed;
}

bool TestRotate3d()
{
    bool passed = true;

    FloatPoint center(0, 0, 0);
    float theta = static_cast<float>(M_PI_2);
    FloatVector axis(0, 0, 1);
    FloatPoint point(1, 0, 0);

    passed = passed && ApproximatelyEqual(RotatePoint3d(center, theta, axis, point), FloatVector(0, 1, 0));
    passed = passed && ApproximatelyEqual(RotatePoint3d(center, -theta, axis, point), FloatVector(0, -1, 0));

    return passed;
}

bool TestPointPlaneNormalDistance()
{
    bool passed = true;

    FloatPoint Q;
    FloatVector N;
    FloatPoint P;

    return passed;
}