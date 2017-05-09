#pragma once

namespace Integrals
{
    template<typename T>
    T GetClosedRangeIncrement(T min, T max, size_t count)
    {
        return (max - min) / (count - 1);
    }

    template<typename rangeType, typename functionType>
    rangeType for_range(rangeType start, rangeType end, functionType func, int nSteps)
    {
        auto delta = GetClosedRangeIncrement(start, end, nSteps);
        for (int i = 0; i < nSteps; ++i)
        {
            func(i, start + delta * i, delta);
        }

        return delta;
    }

    template<typename resultType, typename rangeType, typename integrandFn>
    resultType Integrate(rangeType start, rangeType end, integrandFn integrand, int nSteps, _Out_ rangeType &deltaRange)
    {
        resultType result;

        deltaRange = for_range(start, end, [&](auto i, auto x, auto delta) {
            result += integrand(x) * delta;
        }, nSteps);

        return result;
    }
}
