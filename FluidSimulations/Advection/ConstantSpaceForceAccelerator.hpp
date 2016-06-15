#pragma once

#include "../IMovement.hpp"
#include "../GridDefinitions.hpp"
#include "../ScalarVectorOperations.hpp"

namespace FluidSimulations
{

class ConstantSpaceForceAccelerator
    : public IMovement
{
public:
    typedef std::shared_ptr<FloatsThreeDVector> AccelerationVectorPtr;
    typedef std::shared_ptr<const FloatsThreeDVector> AccelerationVectorConstPtr;

    ConstantSpaceForceAccelerator(
        const FloatsThreeDVectorGridPtr& velocity, 
        const AccelerationVectorConstPtr& acceleration)
        : m_velocity(velocity)
        , m_acceleration(acceleration)
    {
    }

    virtual void move(const FloatType timeInterval) override
    {
        const FloatsThreeDVector offset = multiply(*m_acceleration, timeInterval);

        for (IntegerType i = 0; i < m_velocity->iRes(); ++i)
        {
            for (IntegerType j = 0; j < m_velocity->jRes(); ++j)
            {
                for (IntegerType k = 0; k < m_velocity->kRes(); ++k)
                {
                    sumIn(m_velocity->at(i, j, k), offset);
                }
            }
        }
    }

private:
    const FloatsThreeDVectorGridPtr m_velocity;
    const AccelerationVectorConstPtr m_acceleration;
};

}
