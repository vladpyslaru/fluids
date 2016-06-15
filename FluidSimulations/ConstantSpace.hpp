#pragma once

#include "ScalarTypes.hpp"

namespace FluidSimulations
{

template <typename ValueType>
class ConstantSpace
{
public:
    ConstantSpace(const ValueType& constant)
        : m_constant(constant)
    {}

    ValueType at(const FloatType, const FloatType, const FloatType) const
    {
        return m_constant;
    }

private:
    const ValueType m_constant;
};

}
