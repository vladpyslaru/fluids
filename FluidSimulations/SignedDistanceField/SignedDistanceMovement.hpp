#pragma once

#include "../IMovement.hpp"
#include "ISignedDistanceTracker.hpp"

namespace FluidSimulations
{

namespace SignedDistanceField
{

class SignedDistanceMovement
    : public IMovement
{
public:
    SignedDistanceMovement(
        const IMovementPtr& signedDistanceMovement,
        const ISignedDistanceTrackerPtr& signedDistanceTracker)
        : m_signedDistanceMovement(signedDistanceMovement)
        , m_signedDistanceTracker(signedDistanceTracker)
    {
    }

    virtual void move(const FloatType timeInterval) override
    {
        m_signedDistanceMovement->move(timeInterval);
        m_signedDistanceTracker->track();
    }

private:
    const IMovementPtr m_signedDistanceMovement;
    const ISignedDistanceTrackerPtr m_signedDistanceTracker;
};

}

}