#pragma once

#include "ScalarVectorDefinitions.hpp"

#include <cmath>
#include <algorithm>

namespace FluidSimulations
{

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType>& multiplyIn(ScalarThreeDVector<ScalarType>& buffer, const ScalarType factor)
{
    buffer.x *= factor;
    buffer.y *= factor;
    buffer.z *= factor;

    return buffer;
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType> multiply(const ScalarThreeDVector<ScalarType>& vectorFactor, const ScalarType scalarFactor)
{
    ScalarThreeDVector<ScalarType> result = vectorFactor;

    return multiplyIn(result, scalarFactor);
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType>& multiplyIn(ScalarThreeDVector<ScalarType>& buffer, const ScalarThreeDVector<ScalarType>& factor)
{
    buffer.x *= factor.x;
    buffer.y *= factor.y;
    buffer.z *= factor.z;

    return buffer;
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType> multiply(const ScalarThreeDVector<ScalarType>& left, const ScalarThreeDVector<ScalarType>& right)
{
    ScalarThreeDVector<ScalarType> result = left;

    return multiplyIn(result, right);
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType>& sumIn(ScalarThreeDVector<ScalarType>& buffer, const ScalarType summand)
{
    buffer.x += summand;
    buffer.y += summand;
    buffer.z += summand;

    return buffer;
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType> sum(const ScalarThreeDVector<ScalarType>& vectorSummand, const ScalarType scalarSummand)
{
    ScalarThreeDVector<ScalarType> result = vectorSummand;

    return sumIn(result, scalarSummand);
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType>& sumIn(ScalarThreeDVector<ScalarType>& buffer, const ScalarThreeDVector<ScalarType>& summand)
{
    buffer.x += summand.x;
    buffer.y += summand.y;
    buffer.z += summand.z;

    return buffer;
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType> sum(const ScalarThreeDVector<ScalarType>& left, const ScalarThreeDVector<ScalarType>& right)
{
    ScalarThreeDVector<ScalarType> result = left;

    return sumIn(result, right);
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType>& minusIn(ScalarThreeDVector<ScalarType>& buffer, const ScalarType summand)
{
    buffer.x -= summand;
    buffer.y -= summand;
    buffer.z -= summand;

    return buffer;
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType> minus(const ScalarThreeDVector<ScalarType>& vectorSummand, const ScalarType scalarSummand)
{
    ScalarThreeDVector<ScalarType> result = vectorSummand;

    return minusIn(result, scalarSummand);
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType>& minusIn(ScalarThreeDVector<ScalarType>& buffer, const ScalarThreeDVector<ScalarType>& summand)
{
    buffer.x -= summand.x;
    buffer.y -= summand.y;
    buffer.z -= summand.z;

    return buffer;
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType> minus(const ScalarThreeDVector<ScalarType>& left, const ScalarThreeDVector<ScalarType>& right)
{
    ScalarThreeDVector<ScalarType> result = left;

    return minusIn(result, right);
}

template <typename ScalarType>
inline FloatType norm(const ScalarThreeDVector<ScalarType>& target)
{
    return sqrt(target.x * target.x + target.y * target.y + target.z * target.z);
}

template <typename ScalarType>
inline FloatType distance(const ScalarThreeDVector<ScalarType>& left, ScalarThreeDVector<ScalarType> right)
{
    return norm(minusIn(right, left));
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType>& normalizeIn(ScalarThreeDVector<ScalarType>& target)
{
    const ScalarType targetNorm = norm(target);
    target.x /= targetNorm;
    target.y /= targetNorm;
    target.z /= targetNorm;

    return target;
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType> normalize(const ScalarThreeDVector<ScalarType>& target)
{
    ScalarThreeDVector<ScalarType> normalizedTarget = target;
    return normalizeIn(normalizedTarget);
}

template <typename ScalarType>
inline ScalarThreeDVector<ScalarType> cutByGrid(const ScalarThreeDVector<ScalarType>& position, const IntegersThreeDVector& res)
{
    return FluidSimulations::ScalarThreeDVector<ScalarType>(
        std::max(static_cast<ScalarType>(0), std::min(static_cast<ScalarType>(res.x - 1), position.x)),
        std::max(static_cast<ScalarType>(0), std::min(static_cast<ScalarType>(res.y - 1), position.y)),
        std::max(static_cast<ScalarType>(0), std::min(static_cast<ScalarType>(res.z - 1), position.z)));
}

template <typename ScalarType>
inline bool positionIsOutsideGrid(const ScalarThreeDVector<ScalarType>& position, const IntegersThreeDVector& res)
{
    return position.x < 0 || position.x >(res.x - 1)
        || position.y < 0 || position.y >(res.y - 1)
        || position.z < 0 || position.z >(res.z - 1);
}

template <typename ScalarType>
inline bool positionIsInsideGrid(const ScalarThreeDVector<ScalarType>& position, const IntegersThreeDVector& res)
{
    return !positionIsOutsideGrid(position, res);
}

}