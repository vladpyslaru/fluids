#pragma once

#include "../ScalarVectorOperations.hpp"

namespace FluidSimulations
{

namespace Advection
{

// TODO order

template <typename FloatsThreeDVectorSpaceType>
class RungeKuttaLagrangeTracker
{
public:
    RungeKuttaLagrangeTracker(const FloatsThreeDVectorSpaceType& velocityApproximator)
        : m_velocityApproximator(velocityApproximator)
    {
    }

    inline FloatsThreeDVector traceBack(
        const FloatsThreeDVector& currentPosition,
        const FloatType timeInterval) const
    {
        const FloatsThreeDVector currentVelocity = m_velocityApproximator.at(currentPosition.x, currentPosition.y, currentPosition.z);

        FloatsThreeDVector halfPosition = currentVelocity;
        sumIn(multiplyIn(halfPosition, -0.5f * timeInterval), currentPosition);

        FloatsThreeDVector fullPosition = m_velocityApproximator.at(halfPosition.x, halfPosition.y, halfPosition.z);
        sumIn(multiplyIn(fullPosition, -timeInterval), currentPosition);

        return fullPosition;
    }

private:
    const FloatsThreeDVectorSpaceType m_velocityApproximator;
};

}

}