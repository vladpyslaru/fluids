#pragma once

#include "../GridDefinitions.hpp"
#include "../ScalarVectorOperations.hpp"
#include "ITimeSuggester.hpp"

namespace FluidSimulations
{

namespace Machinery
{

class CflConditionTimeSuggester
    : public ITimeSuggester
{
public:
    CflConditionTimeSuggester(
        const FloatsThreeDVectorGridConstPtr& velocity,
        const FloatType maxAccelerationRate)
        : m_velocity(velocity)
        , m_maxAccelerationRate(maxAccelerationRate)
    {}

    virtual FloatType suggest() const override
    {
        return 5 / (maxSpeed() + sqrt(5 * m_maxAccelerationRate));
    }

private:
    FloatType maxSpeed() const
    {
        FloatType maxSpeed = 0;

        for (IntegerType i = 0; i != m_velocity->iRes(); ++i)
        {
            for (IntegerType j = 0; j != m_velocity->jRes(); ++j)
            {
                for (IntegerType k = 0; k != m_velocity->kRes(); ++k)
                {
                    const FloatType currentSpeed = norm(m_velocity->at(i, j, k));

                    if (currentSpeed > maxSpeed)
                    {
                        maxSpeed = currentSpeed;
                    }
                }
            }
        }

        return maxSpeed;
    }

private:
    const FloatsThreeDVectorGridConstPtr m_velocity;
    const FloatType m_maxAccelerationRate;
};

typedef std::shared_ptr<const ITimeSuggester> ITimeSuggesterConstPtr;

}

}