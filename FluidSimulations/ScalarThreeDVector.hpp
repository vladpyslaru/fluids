#pragma once

#include "ThreeDVector.hpp"

namespace FluidSimulations
{

template <typename ScalarType>
struct ScalarThreeDVector
    : public ThreeDVector<ScalarType>
{
    ScalarThreeDVector()
        : ThreeDVector<ScalarType>(0, 0, 0)
    {}

    ScalarThreeDVector(const ScalarType x, const ScalarType y, const ScalarType z)
        : ThreeDVector<ScalarType>(x, y, z)
    {}
};

}
