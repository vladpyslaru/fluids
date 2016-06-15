#pragma once

#include "../IMovement.hpp"
#include "../GridOperations.hpp"

namespace FluidSimulations
{

namespace Advection
{

template <typename FloatsSpaceType, typename LagrangeTrackerType>
class LagrangeFieldAdvector
    : public IMovement
{
public:
    LagrangeFieldAdvector(
        const FloatsGridPtr& targetGrid,
        const FloatsSpaceType& targetApproximator, 
        const LagrangeTrackerType& lagrangeTracker)
        : m_targetGrid(targetGrid)
        , m_bufferGrid(std::make_shared<FloatsGrid>(targetGrid->iRes(), targetGrid->jRes(), targetGrid->kRes()))
        , m_targetGridApproximator(targetApproximator)
        , m_lagrangeTracker(lagrangeTracker)
    {
    }

    LagrangeFieldAdvector(
        const FloatsGridPtr& targetGrid, 
        const FloatsGridPtr& bufferGrid,
        const FloatsSpaceType& targetApproximator, 
        const LagrangeTrackerType& lagrangeTracker)
        : m_targetGrid(targetGrid)
        , m_bufferGrid(bufferGrid)
        , m_targetGridApproximator(targetApproximator)
        , m_lagrangeTracker(lagrangeTracker)
    {
    }

    LagrangeFieldAdvector(const LagrangeFieldAdvector&) = delete;
    LagrangeFieldAdvector& operator=(const LagrangeFieldAdvector&) = delete;

    virtual void move(const FloatType timeInterval) override
    {
        for (IntegerType i = 0; i < m_bufferGrid->iRes(); ++i)
        {
            for (IntegerType j = 0; j < m_bufferGrid->jRes(); ++j)
            {
                for (IntegerType k = 0; k < m_bufferGrid->kRes(); ++k)
                {
                    const FloatsThreeDVector position = FloatsThreeDVector(static_cast<FloatType>(i), static_cast<FloatType>(j), static_cast<FloatType>(k));
                    const FloatsThreeDVector previousPosition = m_lagrangeTracker.traceBack(position, timeInterval);
                    m_bufferGrid->at(i, j, k) = m_targetGridApproximator.at(previousPosition.x, previousPosition.y, previousPosition.z);
                }
            }
        }

        copyIn<FloatType>(m_bufferGrid, m_targetGrid);
    }

private:
    const FloatsGridPtr m_targetGrid;
    const FloatsGridPtr m_bufferGrid;
    const FloatsSpaceType m_targetGridApproximator;
    const LagrangeTrackerType m_lagrangeTracker;
};

}

}