#pragma once

#include "ScalarVectorDefinitions.hpp"

namespace FluidSimulations
{

template <typename FloatsGridApproximatorType>
class GradientCalculator
{
public:
    GradientCalculator(const FloatsGridApproximatorType& gridApproximator, const FloatType delta)
        : m_gridApproximator(gridApproximator)
        , m_delta(delta)
        , m_denom(1 / (2 * delta))
    {}

    inline FloatsThreeDVector at(const FloatType x, const FloatType y, const FloatType z) const
    {
        return FloatsThreeDVector(
            (m_gridApproximator.at(x + m_delta, y, z) - m_gridApproximator.at(x - m_delta, y, z)) * m_denom,
            (m_gridApproximator.at(x, y + m_delta, z) - m_gridApproximator.at(x, y - m_delta, z)) * m_denom,
            (m_gridApproximator.at(x, y, z + m_delta) - m_gridApproximator.at(x, y, z - m_delta)) * m_denom);
    }

private:
    const FloatsGridApproximatorType m_gridApproximator;
    const FloatType m_delta;
    const FloatType m_denom;
};

}
