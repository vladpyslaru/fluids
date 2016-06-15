#pragma once

#include "ScalarTypes.hpp"

namespace FluidSimulations
{

template <typename FluidPredicateSpaceType, typename SolidPredicateSpaceType>
class AirPredicate
{
public:
    AirPredicate(
        const FluidPredicateSpaceType& fluidPredicate,
        const SolidPredicateSpaceType& solidPredicate)
        : m_fluidPredicate(fluidPredicate)
        , m_solidPredicate(solidPredicate)
    {
    }

    inline bool at(const FloatType x, const FloatType y, const FloatType z) const
    {
        return !m_fluidPredicate.at(x, y, z) && !m_solidPredicate.at(x, y, z);
    }

private:
    const FluidPredicateSpaceType m_fluidPredicate;
    const SolidPredicateSpaceType m_solidPredicate;
};

}