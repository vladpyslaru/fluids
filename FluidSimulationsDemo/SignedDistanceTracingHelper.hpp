#pragma once

#include "FluidSimulationsDemoDefinitions.hpp"

#include <volplay\volplay.h>

namespace FluidSimulationsDemo
{

class SignedDistanceTracingHelper
    : public volplay::SDFNode
{
public:
    SignedDistanceTracingHelper(const FluidSdfSpace& sdf);

    virtual volplay::SDFResult fullEval(const volplay::Vector &x) const override;

private:
    const FluidSdfSpace m_sdf;
};

}
