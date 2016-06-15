#pragma once

#include "GridDefinitions.hpp"

#include <algorithm>

namespace FluidSimulations
{

class LinearFloatsGridApproximator
{
public:
    LinearFloatsGridApproximator(const FloatsGridConstPtr& approximated)
        : m_approximated(approximated)
    {}

    FloatType at(const FloatType x, const FloatType y, const FloatType z) const
    {
        const IntegerType maxI = m_approximated->iRes() - 2;
        const IntegerType maxJ = m_approximated->jRes() - 2;
        const IntegerType maxK = m_approximated->kRes() - 2;

        const IntegerType fX = std::max(std::min(static_cast<IntegerType>(floor(x)), maxI), 0);
        const IntegerType fY = std::max(std::min(static_cast<IntegerType>(floor(y)), maxJ), 0);
        const IntegerType fZ = std::max(std::min(static_cast<IntegerType>(floor(z)), maxK), 0);

        const FloatType& v000 = m_approximated->at(fX, fY, fZ);
        const FloatType& v001 = m_approximated->at(fX, fY, fZ + 1);
        const FloatType& v010 = m_approximated->at(fX, fY + 1, fZ);
        const FloatType& v011 = m_approximated->at(fX, fY + 1, fZ + 1);
        const FloatType& v100 = m_approximated->at(fX + 1, fY, fZ);
        const FloatType& v101 = m_approximated->at(fX + 1, fY, fZ + 1);
        const FloatType& v110 = m_approximated->at(fX + 1, fY + 1, fZ);
        const FloatType& v111 = m_approximated->at(fX + 1, fY + 1, fZ + 1);

        const FloatType xWeight = x - static_cast<FloatType>(fX);
        const FloatType rXWeight = 1 - xWeight;

        const FloatType xI00 = v000 * rXWeight + v100 * xWeight;
        const FloatType xI01 = v001 * rXWeight + v101 * xWeight;
        const FloatType xI10 = v010 * rXWeight + v110 * xWeight;
        const FloatType xI11 = v011 * rXWeight + v111 * xWeight;

        const FloatType yWeight = y - static_cast<FloatType>(fY);
        const FloatType rYWeight = 1 - yWeight;

        const FloatType yI0 = xI00 * rYWeight + xI10 * yWeight;
        const FloatType yI1 = xI01 * rYWeight + xI11 * yWeight;

        const FloatType zWeight = z - static_cast<FloatType>(fZ);
        const FloatType rZWeight = 1 - zWeight;

        return yI0 * rZWeight + yI1 * zWeight;
    }

private:
    const FloatsGridConstPtr m_approximated;
};

}
