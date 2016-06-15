#pragma once

#include <memory>

namespace FluidSimulations
{

namespace Projection
{

class IPressureSolverPreparator
{
public:
    virtual ~IPressureSolverPreparator() {}

    virtual void prepare(const FloatType timeInterval) = 0;
};

typedef std::shared_ptr<IPressureSolverPreparator> IPressureSolverPreparatorPtr;

}

}