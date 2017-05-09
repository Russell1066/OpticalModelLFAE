#pragma once

namespace Integrals
{
    // Get the increment that would be used for a closed range [min, max]
    // This will (within a small amount of error) allow a loop
    // to start at min and end on max.
    template<typename T>
    T GetClosedRangeIncrement(T min, T max, size_t count)
    {
        return (max - min) / (count - 1);
    }

    // Loop over a closed range [start, end]
    // calling a function with the index of the loop, the value at that point, and the delta used
    template<typename rangeType, typename functionType>
    rangeType for_closed_range(rangeType start, rangeType end, functionType func, int nSteps)
    {
        auto delta = GetClosedRangeIncrement(start, end, nSteps);
        for (int i = 0; i < nSteps; ++i, start += delta)
        {
            func(i, start, delta);
        }

        return delta;
    }

    // Integrate the given integrand over the range [start, end] with nSteps steps
    // Returns the sum of all the steps
    template<typename resultType, typename rangeType, typename integrandFn>
    resultType Integrate(rangeType start, rangeType end, integrandFn integrand, int nSteps)
    {
        resultType result;

        for_closed_range(start, end, [&](auto i, auto x, auto delta) {
            result += integrand(x) * delta;
        }, nSteps);

        return result;
    }
}
