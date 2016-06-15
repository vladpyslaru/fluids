#pragma once

#include "GridDefinitions.hpp"

#include <algorithm>

namespace FluidSimulations
{

// TODO split
// TODO debug surface hit

template <
    typename PredicateSpaceType, 
    typename NearestSurfacePointCalculatorType
    >
class LinearObjectVelocityGridApproximator
{
public:
    LinearObjectVelocityGridApproximator(
        const FloatsThreeDVectorGridConstPtr& staggeredVelocity,
        const PredicateSpaceType& interiorPredicate, 
        const NearestSurfacePointCalculatorType& nearestObjectSurfacePoint)
        : m_staggeredVelocity(staggeredVelocity)
        , m_objectPredicate(interiorPredicate)
        , m_nearestObjectSurfacePoint(nearestObjectSurfacePoint)
    {
    }

    FloatsThreeDVector at(const FloatType x, const FloatType y, const FloatType z) const
    {
        if (m_objectPredicate.at(x, y, z))
        {
            return FloatsThreeDVector(
                interpolateAt(x, y, z, iComponent),
                interpolateAt(x, y, z, jComponent),
                interpolateAt(x, y, z, kComponent));
        }

        const FloatsThreeDVector thatPoint = FloatsThreeDVector(x, y, z);

        FloatsThreeDVector nearestFluidPoint = m_nearestObjectSurfacePoint.at(x, y, z);

        // TODO ? small offset for extrapolation

        //minusIn(nearestFluidPoint, thatPoint);

        //const FloatType nearestDirectionNorm = norm(nearestFluidPoint);
        //multiplyIn(nearestFluidPoint, (nearestDirectionNorm + 0.2f) / nearestDirectionNorm);

        //sumIn(nearestFluidPoint, thatPoint);

        return FloatsThreeDVector(
            interpolateAt(nearestFluidPoint.x, nearestFluidPoint.y, nearestFluidPoint.z, iComponent),
            interpolateAt(nearestFluidPoint.x, nearestFluidPoint.y, nearestFluidPoint.z, jComponent),
            interpolateAt(nearestFluidPoint.x, nearestFluidPoint.y, nearestFluidPoint.z, kComponent));
    }

private:
    enum Component
    {
        iComponent,
        jComponent,
        kComponent
    };

    FloatType at(const IntegerType i, const IntegerType j, const IntegerType k, const Component component) const
    {
        const FloatsThreeDVector& velocity = m_staggeredVelocity->at(i, j, k);

        switch (component)
        {
        case iComponent:
        {
            return velocity.x;
        }
        case jComponent:
        {
            return velocity.y;
        }
        case kComponent:
        {
            return velocity.z;
        }
        default:
        {
            return 0;
        }
        }
    }

    FloatType interpolateAt(const FloatType x, const FloatType y, const FloatType z, const Component component) const
    {
        const FloatType uX = component == iComponent ? (x + 0.5f) : x;
        const FloatType uY = component == jComponent ? (y + 0.5f) : y;
        const FloatType uZ = component == kComponent ? (z + 0.5f) : z;

        const IntegerType maxI = m_staggeredVelocity->iRes() - 2;
        const IntegerType maxJ = m_staggeredVelocity->jRes() - 2;
        const IntegerType maxK = m_staggeredVelocity->kRes() - 2;

        const IntegerType fX = std::max(std::min(static_cast<IntegerType>(floor(uX)), maxI), 0);
        const IntegerType fY = std::max(std::min(static_cast<IntegerType>(floor(uY)), maxJ), 0);
        const IntegerType fZ = std::max(std::min(static_cast<IntegerType>(floor(uZ)), maxK), 0);

        const FloatType rX = component == iComponent ? (fX - 0.5f) : fX;
        const FloatType rY = component == jComponent ? (fY - 0.5f) : fY;
        const FloatType rZ = component == kComponent ? (fZ - 0.5f) : fZ;

        const FloatType v000 = at(fX, fY, fZ, component);
        const FloatType v001 = at(fX, fY, fZ + 1, component);
        const FloatType v010 = at(fX, fY + 1, fZ, component);
        const FloatType v011 = at(fX, fY + 1, fZ + 1, component);
        const FloatType v100 = at(fX + 1, fY, fZ, component);
        const FloatType v101 = at(fX + 1, fY, fZ + 1, component);
        const FloatType v110 = at(fX + 1, fY + 1, fZ, component);
        const FloatType v111 = at(fX + 1, fY + 1, fZ + 1, component);

        const bool p000 = m_objectPredicate.at(rX, rY, rZ);
        const bool p001 = m_objectPredicate.at(rX, rY, rZ + 1);
        const bool p010 = m_objectPredicate.at(rX, rY + 1, rZ);
        const bool p011 = m_objectPredicate.at(rX, rY + 1, rZ + 1);
        const bool p100 = m_objectPredicate.at(rX + 1, rY, rZ);
        const bool p101 = m_objectPredicate.at(rX + 1, rY, rZ + 1);
        const bool p110 = m_objectPredicate.at(rX + 1, rY + 1, rZ);
        const bool p111 = m_objectPredicate.at(rX + 1, rY + 1, rZ + 1);

        const FloatType xWeight = uX - static_cast<FloatType>(fX);
        const FloatType rXWeight = 1 - xWeight;

        const FloatType xI00 = p000 && p100 ? v000 * rXWeight + v100 * xWeight
            : p000 && !p100 ? v000
            : !p000 && p100 ? v100
            : 0;

        const bool pI00 = p000 || p100;

        const FloatType xI01 = p001 && p101 ? v001 * rXWeight + v101 * xWeight
            : p001 && !p101 ? v001
            : !p001 && p101 ? v101
            : 0;

        const bool pI01 = p001 || p101;

        const FloatType xI10 = p010 && p110 ? v010 * rXWeight + v110 * xWeight
            : p010 && !p110 ? v010
            : !p010 && p110 ? v110
            : 0;

        const bool pI10 = p010 || p110;

        const FloatType xI11 = p011 && p111 ? v011 * rXWeight + v111 * xWeight
            : p011 && !p111 ? v011
            : !p011 && p111 ? v111
            : 0;

        const bool pI11 = p011 || p111;

        const FloatType yWeight = uY - static_cast<FloatType>(fY);
        const FloatType rYWeight = 1 - yWeight;

        const FloatType yI0 = pI00 && pI10 ? xI00 * rYWeight + xI10 * yWeight
            : pI00 && !pI10 ? xI00
            : !pI00 && pI10 ? xI10
            : 0;

        const bool pI0 = pI00 || pI10;

        const FloatType yI1 = pI01 && pI11 ? xI01 * rYWeight + xI11 * yWeight
            : pI01 && !pI11 ? xI01
            : !pI01 && pI11 ? xI11
            : 0;

        const bool pI1 = pI01 || pI11;

        const FloatType zWeight = uZ - static_cast<FloatType>(fZ);
        const FloatType rZWeight = 1 - zWeight;

        return pI0 && pI1 ? yI0 * rZWeight + yI1 * zWeight
            : pI0 && !pI1 ? yI0
            : !pI0 && pI1 ? yI1
            : 0;
    }

private:
    const FloatsThreeDVectorGridConstPtr m_staggeredVelocity;
    const PredicateSpaceType m_objectPredicate;
    const NearestSurfacePointCalculatorType m_nearestObjectSurfacePoint;
};

}
