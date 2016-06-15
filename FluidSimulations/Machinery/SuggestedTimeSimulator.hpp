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

class SuggestedTimeSimulator
    : public ISimulator
{
public:
    SuggestedTimeSimulator(
        const ITimeSuggesterConstPtr timeSuggester, 
        const std::vector<IMovementPtr>& movements)
        : m_timeSuggester(timeSuggester)
        , m_movements(movements)
    {}

    virtual void step() override
    {
        const FloatType suggestedTime = m_timeSuggester->suggest();

        std::for_each(m_movements.cbegin(), m_movements.cend(),
            [suggestedTime](const FluidSimulations::IMovementPtr& movement) -> void
            {
                movement->move(suggestedTime);
            });
    }

private:
    const ITimeSuggesterConstPtr m_timeSuggester;
    const std::vector<IMovementPtr> m_movements;
};

}

}