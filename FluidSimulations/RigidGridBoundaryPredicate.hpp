#pragma once

#include "ScalarVectorDefinitions.hpp"

namespace FluidSimulations
{

class RigidGridBoundaryPredicate
{
public:
    RigidGridBoundaryPredicate(
        const IntegersThreeDVector& resolution, 
        const FloatType boundaryOffset = static_cast<FloatType>(0))
        : m_iRes(resolution.x)
        , m_jRes(resolution.y)
        , m_kRes(resolution.z)
        , m_boundaryOffset(boundaryOffset)
    {
    }

    inline bool at(const FloatType x, const FloatType y, const FloatType z) const
    {
        return (x <= m_boundaryOffset) || (x >= (m_iRes - 1 - m_boundaryOffset))
            || (y <= m_boundaryOffset) || (y >= (m_jRes - 1 - m_boundaryOffset))
            || (z <= m_boundaryOffset) || (z >= (m_kRes - 1 - m_boundaryOffset));
    }

private:
    const IntegerType m_iRes;
    const IntegerType m_jRes;
    const IntegerType m_kRes;
    const FloatType m_boundaryOffset;
};

}
