#pragma once

#include "../IMovement.hpp"
#include "../GridOperations.hpp"

namespace FluidSimulations
{

namespace Advection
{

template <typename FloatsThreeDVectorSpaceType, typename LagrangeTrackerType>
class LagrangeVelocityAdvector
    : public IMovement
{
public:
    LagrangeVelocityAdvector(
        const FloatsThreeDVectorGridPtr& targetVelocityGrid,
        const FloatsThreeDVectorSpaceType& velocityApproximator, 
        const LagrangeTrackerType& lagrangeTracker)
        : m_targetVelocityGrid(targetVelocityGrid)
        , m_bufferVelocityGrid(std::make_shared<FloatsThreeDVectorGrid>(targetVelocityGrid->iRes(), targetVelocityGrid->jRes(), targetVelocityGrid->kRes()))
        , m_velocityApproximator(velocityApproximator)
        , m_lagrangeTracker(lagrangeTracker)
    {
    }

    LagrangeVelocityAdvector(
        const FloatsThreeDVectorGridPtr& targetVelocityGrid, 
        const FloatsThreeDVectorGridPtr& bufferVelocityGrid, 
        const FloatsThreeDVectorSpaceType& velocityApproximator, 
        const LagrangeTrackerType& lagrangeTracker)
        : m_targetVelocityGrid(targetVelocityGrid)
        , m_bufferVelocityGrid(bufferVelocityGrid)
        , m_velocityApproximator(velocityApproximator)
        , m_lagrangeTracker(lagrangeTracker)
    {
    }

    LagrangeVelocityAdvector(const LagrangeVelocityAdvector&) = delete;
    LagrangeVelocityAdvector& operator=(const LagrangeVelocityAdvector&) = delete;

    virtual void move(const FloatType timeInterval) override
    {
        for (IntegerType i = 0; i < m_bufferVelocityGrid->iRes(); ++i)
        {
            for (IntegerType j = 0; j < m_bufferVelocityGrid->jRes(); ++j)
            {
                for (IntegerType k = 0; k < m_bufferVelocityGrid->kRes(); ++k)
                {
                    const FloatsThreeDVector iPosition = FloatsThreeDVector(static_cast<FloatType>(i) - 0.5f, static_cast<FloatType>(j), static_cast<FloatType>(k));
                    const FloatsThreeDVector iPreviousPosition = m_lagrangeTracker.traceBack(iPosition, timeInterval);
                    const FloatType iPreviousSpeed = m_velocityApproximator.at(iPreviousPosition.x, iPreviousPosition.y, iPreviousPosition.z).x;

                    const FloatsThreeDVector jPosition = FloatsThreeDVector(static_cast<FloatType>(i), static_cast<FloatType>(j) - 0.5f, static_cast<FloatType>(k));
                    const FloatsThreeDVector jPreviousPosition = m_lagrangeTracker.traceBack(jPosition, timeInterval);
                    const FloatType jPreviousSpeed = m_velocityApproximator.at(jPreviousPosition.x, jPreviousPosition.y, jPreviousPosition.z).y;

                    const FloatsThreeDVector kPosition = FloatsThreeDVector(static_cast<FloatType>(i), static_cast<FloatType>(j), static_cast<FloatType>(k) - 0.5f);
                    const FloatsThreeDVector kPreviousPosition = m_lagrangeTracker.traceBack(kPosition, timeInterval);
                    const FloatType kPreviousSpeed = m_velocityApproximator.at(kPreviousPosition.x, kPreviousPosition.y, kPreviousPosition.z).z;

                    m_bufferVelocityGrid->at(i, j, k) = FloatsThreeDVector(iPreviousSpeed, jPreviousSpeed, kPreviousSpeed);
                }
            }
        }

        copyIn<FloatsThreeDVector>(m_bufferVelocityGrid, m_targetVelocityGrid);
    }

private:
    const FloatsThreeDVectorGridPtr m_targetVelocityGrid;
    const FloatsThreeDVectorGridPtr m_bufferVelocityGrid;
    const FloatsThreeDVectorSpaceType m_velocityApproximator;
    const LagrangeTrackerType m_lagrangeTracker;
};

}

}