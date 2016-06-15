#pragma once

#include <memory>

namespace FluidSimulations
{

namespace SignedDistanceField
{

class ISignedDistanceTracker
{
public:
    virtual ~ISignedDistanceTracker() {}

    virtual void track() = 0;
};

typedef std::shared_ptr<ISignedDistanceTracker> ISignedDistanceTrackerPtr;

}

}