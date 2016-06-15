#pragma once

namespace FluidSimulations
{

template <typename ValueType>
struct FourDVector
{
    ValueType x;
    ValueType y;
    ValueType z;
    ValueType w;

    FourDVector(const ValueType& x, const ValueType& y, const ValueType& z, const ValueType& w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {}
};

}