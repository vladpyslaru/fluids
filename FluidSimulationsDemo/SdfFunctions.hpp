#pragma once

#include "FluidSimulationsDemoDefinitions.hpp"

namespace FluidSimulationsDemo
{

FloatSpaceFunctor waterBallSdf(const FluidSimulations::FloatsThreeDVector& center, 
    const FluidSimulations::FloatType& radius);

}