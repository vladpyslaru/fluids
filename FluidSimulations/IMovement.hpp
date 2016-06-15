#pragma once

#include "ScalarTypes.hpp"

#include <memory>

namespace FluidSimulations
{

class IMovement
{
public:
    virtual ~IMovement() {}

    virtual void move(const FloatType timeInterval) = 0;
};

typedef std::shared_ptr<IMovement> IMovementPtr;

}
