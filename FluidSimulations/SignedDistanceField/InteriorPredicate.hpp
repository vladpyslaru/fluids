#pragma once

#include "../GridDefinitions.hpp"

namespace FluidSimulations
{

namespace SignedDistanceField
{

template <typename FloatsSpaceType>
class InteriorPredicate
{
public:
    InteriorPredicate(const FloatsSpaceType& signedDistance)
        : m_signedDistance(signedDistance)
    {}

    inline bool at(const FloatType x, const FloatType y, const FloatType z) const
    {
        return m_signedDistance.at(x, y, z) <= static_cast<FloatType>(0);
    }

private:
    const FloatsSpaceType m_signedDistance;
};

}

}