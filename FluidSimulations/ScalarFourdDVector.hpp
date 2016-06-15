#pragma once

#include "FourDVector.hpp"

namespace FluidSimulations
{

template <typename ScalarType>
struct ScalarFourdDVector
    : public FourDVector<ScalarType>
{
    ScalarFourdDVector()
        : FourDVector<ScalarType>(0, 0, 0, 0)
    {}

    ScalarFourdDVector(const ScalarType x, const ScalarType y, const ScalarType z, const ScalarType w)
        : FourDVector<ScalarType>(x, y, z, w)
    {}
};

}
