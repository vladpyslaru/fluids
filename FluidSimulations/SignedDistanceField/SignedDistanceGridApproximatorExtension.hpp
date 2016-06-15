#pragma once

#include "../ScalarVectorOperations.hpp"
#include "../RigidGridBoundary.hpp"
#include "NearestSurfacePointCalculator.hpp"

namespace FluidSimulations
{

namespace SignedDistanceField
{

template <typename FloatsSpaceType>
class SignedDistanceGridApproximatorExtension
{
public:
    SignedDistanceGridApproximatorExtension(
        const FloatsSpaceType& signedDistance,
        const IntegersThreeDVector& resolution)
        : m_signedDistanceSpace(signedDistance)
        , m_nearestSurfacePoint(NearestSurfacePointCalculator<FloatsSpaceType>(signedDistance))
        , m_resolution(resolution)
    {
    }

    inline FloatType at(const FloatType x, const FloatType y, const FloatType z) const
    {
        const FloatsThreeDVector p = FloatsThreeDVector(x, y, z);

        const FloatsThreeDVector b = cutByGrid(p, m_resolution);

        const FloatsThreeDVector s = m_nearestSurfacePoint.at(b.x, b.y, b.z);

        return distance(s, p);
    }

    static inline RigidGridBoundary<FloatType, FloatsSpaceType, SignedDistanceGridApproximatorExtension> wrapSignedDistance(
        const FloatsSpaceType& signedDistance, const IntegersThreeDVector& resolution, const FloatType offset)
    {
        const SignedDistanceGridApproximatorExtension externalRigidSignedDistance(signedDistance, resolution);

        return RigidGridBoundary<FloatType, FloatsSpaceType, SignedDistanceGridApproximatorExtension>(
            signedDistance, externalRigidSignedDistance, resolution, offset);
    }

private:
    const FloatsSpaceType m_signedDistanceSpace;
    const NearestSurfacePointCalculator<FloatsSpaceType> m_nearestSurfacePoint;
    const IntegersThreeDVector m_resolution;
};

}

}