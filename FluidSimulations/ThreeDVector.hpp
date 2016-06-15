#pragma once

namespace FluidSimulations
{

template <typename ValueType>
struct ThreeDVector
{
    ValueType x;
    ValueType y;
    ValueType z;

    ThreeDVector(const ValueType& x, const ValueType& y, const ValueType& z)
        : x(x)
        , y(y)
        , z(z)
    {}
};

}