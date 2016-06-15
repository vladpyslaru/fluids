#pragma once

#include "../IMovement.hpp"
#include "../ISimulator.hpp"
#include "ITimeSuggester.hpp"

#include <vector>
#include <algorithm>

namespace FluidSimulations
{

namespace Machinery
{

class ConstantTimeSimulator
    : public ISimulator
{
public:
    ConstantTimeSimulator(
        const ITimeSuggesterConstPtr timeSuggester,
        const std::vector<IMovementPtr>& movements,
        const FloatType timeInterval)
        : m_timeSuggester(timeSuggester)
        , m_movements(movements)
        , m_timeInterval(timeInterval)
    {}

    virtual void step() override
    {
        FloatType residualTimeInterval = m_timeInterval;
        while (residualTimeInterval > 0)
        {
            const FloatType nextTimeInterval = std::min(m_timeSuggester->suggest(), residualTimeInterval);

            move(nextTimeInterval);

            residualTimeInterval -= nextTimeInterval;
        }
    }

private:
    void move(const FloatType timeInterval)
    {
        std::for_each(m_movements.cbegin(), m_movements.cend(),
            [timeInterval](const FluidSimulations::IMovementPtr& movement) -> void
        {
            movement->move(timeInterval);
        });
    }

private:
    const ITimeSuggesterConstPtr m_timeSuggester;
    const std::vector<IMovementPtr> m_movements;
    const FloatType m_timeInterval;
};

}

}