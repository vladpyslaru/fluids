#pragma once

#include "../GridOperations.hpp"
#include "../ScalarVectorOperations.hpp"
#include "ISignedDistanceTracker.hpp"
#include "NearestSurfacePointCalculator.hpp"

#include <cmath>

namespace FluidSimulations
{

namespace SignedDistanceField
{

template <typename FloatsSpaceType>
class SignedDistanceSweeper
    : public ISignedDistanceTracker
{
public:
    SignedDistanceSweeper(
        const FloatsGridPtr& signedDistance,
        const FloatsSpaceType& signedDistanceApproximator)
        : m_signedDistance(signedDistance)
        , m_nearestSurfacePoint(NearestSurfacePointCalculator<FloatsSpaceType>(signedDistanceApproximator))
        , m_knownPoints(std::make_shared<Grid<bool>>(signedDistance->iRes(), signedDistance->jRes(), signedDistance->kRes()))
        , m_closestSurfacePoints(std::make_shared<Grid<FloatsThreeDVector>>(signedDistance->iRes(), signedDistance->jRes(), signedDistance->kRes()))
    {
    }

    SignedDistanceSweeper(const SignedDistanceSweeper&) = delete;
    SignedDistanceSweeper& operator=(const SignedDistanceSweeper&) = delete;

    virtual void track() override
    {
        determineBoundarySet();

        sweep();
        sweep();
    }

private:
    static inline bool differentSigns(const FloatType left, const FloatType& right)
    {
        return left < 0 && right >= 0 || left >= 0 && right < 0;
    }

    static inline FloatType distance(const FloatsThreeDVector& v, const IntegerType i, const IntegerType j, const IntegerType k)
    {
        return FluidSimulations::distance(v, FloatsThreeDVector(static_cast<FloatType>(i), static_cast<FloatType>(j), static_cast<FloatType>(k)));
    }

    void trackPoint(const FloatsThreeDVector& closestSurfacePoint,
        const IntegerType i, const IntegerType j, const IntegerType k)
    {
        const FloatType distanceToSurface = distance(closestSurfacePoint, i, j, k);

        FloatType& signedDistance = m_signedDistance->at(i, j, k);
        bool& pointIsKnown = m_knownPoints->at(i, j, k);

        if (pointIsKnown && (distanceToSurface < abs(signedDistance)) || !pointIsKnown)
        {
            m_closestSurfacePoints->at(i, j, k) = closestSurfacePoint;
            signedDistance = (signedDistance <= 0) ? -distanceToSurface : distanceToSurface;
            pointIsKnown = true;
        }
    }

    void encloseSurface(const IntegerType ic, const IntegerType jc, const IntegerType kc,
        const IntegerType in, const IntegerType jn, const IntegerType kn)
    {
        const FloatType& vc = m_signedDistance->at(ic, jc, kc);
        const FloatType& vn = m_signedDistance->at(in, jn, kn);

        if (!differentSigns(vc, vn))
        {
            return;
        }

        const FloatsThreeDVector sc = m_nearestSurfacePoint.at(static_cast<FloatType>(ic), static_cast<FloatType>(jc), static_cast<FloatType>(kc));
        const FloatsThreeDVector sn = m_nearestSurfacePoint.at(static_cast<FloatType>(in), static_cast<FloatType>(jn), static_cast<FloatType>(kn));

        trackPoint(sc, ic, jc, kc);
        trackPoint(sn, in, jn, kn);
    }

    void determineBoundarySet()
    {
        setValues(m_knownPoints, false);

        for (IntegerType i = 0; i < m_closestSurfacePoints->iRes() - 1; ++i)
        {
            for (IntegerType j = 0; j < m_closestSurfacePoints->jRes() - 1; ++j)
            {
                for (IntegerType k = 0; k < m_closestSurfacePoints->kRes() - 1; ++k)
                {
                    encloseSurface(i, j, k, i + 1, j, k);
                    encloseSurface(i, j, k, i, j + 1, k);
                    encloseSurface(i, j, k, i, j, k + 1);
                }
            }
        }
    }

    void sweepFromNeighbour(const IntegerType ic, const IntegerType jc, const IntegerType kc,
        const IntegerType in, const IntegerType jn, const IntegerType kn)
    {
        bool& neighbourIsKnown = m_knownPoints->at(in, jn, kn);
        if (!neighbourIsKnown)
        {
            return;
        }

        const FloatsThreeDVector& neighbourClosestPoint = m_closestSurfacePoints->at(in, jn, kn);

        trackPoint(neighbourClosestPoint, ic, jc, kc);

        neighbourIsKnown = distance(neighbourClosestPoint, ic, jc, kc) >= abs(m_signedDistance->at(in, jn, kn));
    }

    void sweepPoint(const IntegerType i, const IntegerType j, const IntegerType k)
    {
        if (m_knownPoints->at(i, j, k))
        {
            return;
        }

        sweepFromNeighbour(i, j, k, i, j, k);

        if (i > 0)
        {
            sweepFromNeighbour(i, j, k, i - 1, j, k);
        }

        if (i < m_knownPoints->iRes() - 1)
        {
            sweepFromNeighbour(i, j, k, i + 1, j, k);
        }

        if (j > 0)
        {
            sweepFromNeighbour(i, j, k, i, j - 1, k);
        }

        if (j < m_knownPoints->jRes() - 1)
        {
            sweepFromNeighbour(i, j, k, i, j + 1, k);
        }

        if (k > 0)
        {
            sweepFromNeighbour(i, j, k, i, j, k - 1);
        }

        if (k < m_knownPoints->kRes() - 1)
        {
            sweepFromNeighbour(i, j, k, i, j, k + 1);
        }
    }

    void sweep()
    {
        for (IntegerType i = 0; i < m_knownPoints->iRes(); ++i)
        {
            for (IntegerType j = 0; j < m_knownPoints->jRes(); ++j)
            {
                for (IntegerType k = 0; k < m_knownPoints->kRes(); ++k)
                {
                    sweepPoint(i, j, k);
                }
            }
        }

        for (IntegerType i = 0; i < m_knownPoints->iRes(); ++i)
        {
            for (IntegerType j = 0; j < m_knownPoints->jRes(); ++j)
            {
                for (IntegerType k = m_knownPoints->kRes() - 1; k >= 0; --k)
                {
                    sweepPoint(i, j, k);
                }
            }
        }

        for (IntegerType i = 0; i < m_knownPoints->iRes(); ++i)
        {
            for (IntegerType j = m_knownPoints->jRes() - 1; j >= 0; --j)
            {
                for (IntegerType k = 0; k < m_knownPoints->kRes(); ++k)
                {
                    sweepPoint(i, j, k);
                }
            }
        }

        for (IntegerType i = 0; i < m_knownPoints->iRes(); ++i)
        {
            for (IntegerType j = m_knownPoints->jRes() - 1; j >= 0; --j)
            {
                for (IntegerType k = m_knownPoints->kRes() - 1; k >= 0; --k)
                {
                    sweepPoint(i, j, k);
                }
            }
        }

        for (IntegerType i = m_knownPoints->iRes() - 1; i >= 0; --i)
        {
            for (IntegerType j = 0; j < m_knownPoints->jRes(); ++j)
            {
                for (IntegerType k = 0; k < m_knownPoints->kRes(); ++k)
                {
                    sweepPoint(i, j, k);
                }
            }
        }

        for (IntegerType i = m_knownPoints->iRes() - 1; i >= 0; --i)
        {
            for (IntegerType j = 0; j < m_knownPoints->jRes(); ++j)
            {
                for (IntegerType k = m_knownPoints->kRes() - 1; k >= 0; --k)
                {
                    sweepPoint(i, j, k);
                }
            }
        }

        for (IntegerType i = m_knownPoints->iRes() - 1; i >= 0; --i)
        {
            for (IntegerType j = m_knownPoints->jRes() - 1; j >= 0; --j)
            {
                for (IntegerType k = 0; k < m_knownPoints->kRes(); ++k)
                {
                    sweepPoint(i, j, k);
                }
            }
        }

        for (IntegerType i = m_knownPoints->iRes() - 1; i >= 0; --i)
        {
            for (IntegerType j = m_knownPoints->jRes() - 1; j >= 0; --j)
            {
                for (IntegerType k = m_knownPoints->kRes() - 1; k >= 0; --k)
                {
                    sweepPoint(i, j, k);
                }
            }
        }
    }

private:
    const FloatsGridPtr m_signedDistance;
    const NearestSurfacePointCalculator<FloatsSpaceType> m_nearestSurfacePoint;

    const PredicateGridPtr m_knownPoints;
    const FloatsThreeDVectorGridPtr m_closestSurfacePoints;
};

}

}