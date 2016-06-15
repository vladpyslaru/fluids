#pragma once

#include "../GradientCalculator.hpp"
#include "../ScalarVectorOperations.hpp"

namespace FluidSimulations
{

namespace SignedDistanceField
{

template <typename FloatsSpaceType>
class NearestSurfacePointCalculator
{
public:
    NearestSurfacePointCalculator(
        const FloatsSpaceType& signedDistance, 
        const FloatType gradientDelta = 0.0001f)
        : m_signedDistance(signedDistance)
        , m_signedDistanceGradient(GradientCalculator<FloatsSpaceType>(
            signedDistance, gradientDelta))
    {
    }

    inline FloatsThreeDVector at(const FloatType x, const FloatType y, const FloatType z) const
    {
        FloatsThreeDVector closestSurfacePoint = m_signedDistanceGradient.at(x, y, z);

        normalizeIn(closestSurfacePoint);
        multiplyIn(closestSurfacePoint, -m_signedDistance.at(x, y, z));
        sumIn(closestSurfacePoint, FloatsThreeDVector(x, y, z));

        return closestSurfacePoint;
    }

private:
    const FloatsSpaceType m_signedDistance;
    const GradientCalculator<FloatsSpaceType> m_signedDistanceGradient;
};

}

}