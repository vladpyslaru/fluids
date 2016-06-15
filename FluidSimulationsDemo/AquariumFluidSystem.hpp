#pragma once

#include "FluidSimulationsDemoDefinitions.hpp"

#include <FluidSimulations\FluidSimulations.hpp>
#include <vector>

namespace FluidSimulationsDemo
{

class AquariumFluidSystem
{
public:
    static std::shared_ptr<AquariumFluidSystem> build(const int iRes, const int jRes, const int kRes,
        const FluidSimulations::FloatType gravitationAcceleration, const FloatSpaceFunctor& sdf, 
        const FluidSimulations::FloatType timeInterval);

    AquariumFluidSystem(const AquariumFluidSystem&) = delete;
    AquariumFluidSystem& operator=(const AquariumFluidSystem&) = delete;

    const FluidSdfSpace& fluidSdf() const;

    const FluidSimulations::ISimulatorPtr simulator() const;
    
private:
    AquariumFluidSystem(
        const FluidSdfSpace& fluidSdf,
        const FluidSimulations::ISimulatorPtr& simulator);

private:
    const FluidSdfSpace m_fluidSdf;
    const FluidSimulations::ISimulatorPtr m_simulator;
    
};

}