#pragma once

#include "ScalarTypes.hpp"

#include <memory>

namespace FluidSimulations
{

class ISimulator
{
public:
    virtual ~ISimulator() {}

    virtual void step() = 0;
};

typedef std::shared_ptr<ISimulator> ISimulatorPtr;

}
