#pragma once

#include <FluidSimulations\FluidSimulations.hpp>
#include <functional>

namespace FluidSimulationsDemo
{

typedef std::function
<   FluidSimulations::FloatType(
        const FluidSimulations::FloatType, 
        const FluidSimulations::FloatType, 
        const FluidSimulations::FloatType)
> 
FloatSpaceFunctor;

typedef FluidSimulations::LinearFloatsGridApproximator InternalFluidSdfSpace;

typedef FluidSimulations::RigidGridBoundary
<
    FluidSimulations::FloatType,
    InternalFluidSdfSpace, 
    FluidSimulations::SignedDistanceField::SignedDistanceGridApproximatorExtension<InternalFluidSdfSpace>
>
FluidSdfSpace;

typedef FluidSimulations::SignedDistanceField::InteriorPredicate<FluidSdfSpace> FluidPredicate;

typedef FluidSimulations::LinearObjectVelocityGridApproximator
<
    FluidSimulations::SignedDistanceField::InteriorPredicate<FluidSdfSpace>,
    FluidSimulations::SignedDistanceField::NearestSurfacePointCalculator<FluidSdfSpace>
>
InternalVelocitySpace;

typedef FluidSimulations::RigidGridBoundary
<
    FluidSimulations::FloatsThreeDVector,
    InternalVelocitySpace,
    FluidSimulations::ConstantSpace<FluidSimulations::FloatsThreeDVector>
>
VelocitySpace;

}
