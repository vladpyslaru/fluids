#pragma once

#include "RigidGridBoundary.hpp"
#include "ConstantSpace.hpp"
#include "ScalarVectorDefinitions.hpp"

namespace FluidSimulations
{

template <typename FloatsThreeDVectorSpaceType>
RigidGridBoundary<FloatsThreeDVector, FloatsThreeDVectorSpaceType, ConstantSpace<FloatsThreeDVector>> borderVelocity(
    const FloatsThreeDVectorSpaceType& velocity, const IntegersThreeDVector& resolution, const FloatType offset)
{
    const ConstantSpace<FloatsThreeDVector> externalZeroVelocitySpace(FloatsThreeDVector(0, 0, 0));

    return RigidGridBoundary<FloatsThreeDVector, FloatsThreeDVectorSpaceType, ConstantSpace<FloatsThreeDVector>>(
        velocity, externalZeroVelocitySpace, resolution, offset);
}

}