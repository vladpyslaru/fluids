#pragma once

#include "ScalarVectorDefinitions.hpp"

namespace FluidSimulations
{

template <typename ValueType, typename InternalFloatsSpaceType, typename ExternalFloatsSpaceType>
class RigidGridBoundary
{
public:
    RigidGridBoundary(const InternalFloatsSpaceType& internalSpace, const ExternalFloatsSpaceType& externalSpace,
        const IntegersThreeDVector& resolution, const FloatType boundaryOffset = static_cast<FloatType>(0))
        : m_internalSpace(internalSpace)
        , m_externalSpace(externalSpace)
        , m_iRes(resolution.x)
        , m_jRes(resolution.y)
        , m_kRes(resolution.z)
        , m_boundaryOffset(boundaryOffset)
    {
    }

    inline ValueType at(const FloatType x, const FloatType y, const FloatType z) const
    {
        return isInside(x, y, z)
            ? m_internalSpace.at(x, y, z)
            : m_externalSpace.at(x, y, z);
    }

private:
    inline bool isInside(const FloatType x, const FloatType y, const FloatType z) const
    {
        return (x > m_boundaryOffset) && (x < (m_iRes - 1 - m_boundaryOffset))
            && (y > m_boundaryOffset) && (y < (m_jRes - 1 - m_boundaryOffset))
            && (z > m_boundaryOffset) && (z < (m_kRes - 1 - m_boundaryOffset));
    }

private:
    const InternalFloatsSpaceType m_internalSpace;
    const ExternalFloatsSpaceType m_externalSpace;
    const IntegerType m_iRes;
    const IntegerType m_jRes;
    const IntegerType m_kRes;
    const FloatType m_boundaryOffset;
};

}