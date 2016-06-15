#pragma once

#include "../ScalarTypes.hpp"

#include <memory>

namespace FluidSimulations
{

namespace Projection
{

class IPressureSolver
{
public:
    virtual ~IPressureSolver() {}

    virtual void solve() = 0;
};

typedef std::shared_ptr<IPressureSolver> IPressureSolverPtr;

}

}