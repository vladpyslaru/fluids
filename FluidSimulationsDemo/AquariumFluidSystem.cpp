#include "AquariumFluidSystem.hpp"

#include <algorithm>

using FluidSimulations::FloatType;
using FluidSimulations::IntegerType;
using FluidSimulations::FloatsThreeDVector;

namespace FluidSimulationsDemo
{

namespace
{

FluidSimulations::FloatsGridPtr buildGrid(const int iRes, const int jRes, const int kRes,
    const FloatSpaceFunctor& sdf)
{
    FluidSimulations::FloatsGridPtr targetGrid = std::make_shared<FluidSimulations::Grid<FloatType>>(iRes, jRes, kRes);

    for (IntegerType i = 0; i != iRes; ++i)
    {
        for (IntegerType j = 0; j != jRes; ++j)
        {
            for (IntegerType k = 0; k != kRes; ++k)
            {
                targetGrid->at(i, j, k) = sdf(static_cast<FloatType>(i), static_cast<FloatType>(j), static_cast<FloatType>(k));
            }
        }
    }

    return targetGrid;
}

FluidSimulations::FloatsThreeDVectorGridPtr bluildVelocityGrid(const FluidSimulations::IntegersThreeDVector& res)
{
    return std::make_shared<FluidSimulations::Grid<FloatsThreeDVector>>(res.x + 1, res.y + 1, res.z + 1, FloatsThreeDVector());
}

FluidSimulations::IMovementPtr buildPressureImposer(const FluidSimulations::IntegersThreeDVector& resolution,
    const FluidSdfSpace& sdf, const FluidSimulations::FloatsThreeDVectorGridPtr& velocity, const FloatType solidOffset)
{
    using namespace FluidSimulations;

    typedef Projection::GridAllignedPressureSolvePreparator
    <
            FluidPredicate, RigidGridBoundaryPredicate,
            AirPredicate<FluidPredicate,
            RigidGridBoundaryPredicate>,
            ConstantSpace<FloatsThreeDVector>
    >
    PreparatorType;

    typedef Projection::MiccgZeroSolver<FluidPredicate> SolverType;

    typedef Projection::VelocityProjectionMovement
    <
        FluidPredicate, RigidGridBoundaryPredicate, ConstantSpace<FloatsThreeDVector>
    >
    PressureMovementType;

    const FluidPredicate fluidPredicate(sdf);
    const RigidGridBoundaryPredicate solidPredicate(resolution, solidOffset);
    const AirPredicate<FluidPredicate, RigidGridBoundaryPredicate> airPredicate(fluidPredicate, solidPredicate);

    const FloatType fluidDensity = static_cast<FloatType>(1000);
    const FloatType tolerance = 0.00001f;
    const IntegerType maxIterationsCount = 1000;

    const ConstantSpace<FloatsThreeDVector> solidVelocity(FloatsThreeDVector(0, 0, 0));

    const FloatsFourDVectorGridPtr coefficients = std::make_shared<FloatsFourDVectorGrid>(resolution.x, resolution.y, resolution.z);
    const FloatsGridPtr rhs = std::make_shared<FloatsGrid>(resolution.x, resolution.y, resolution.z);
    const FloatsGridPtr pressure = std::make_shared<FloatsGrid>(resolution.x, resolution.y, resolution.z);

    const Projection::IPressureSolverPreparatorPtr coefficientCalculator = std::make_shared<PreparatorType>
        (resolution, velocity, fluidPredicate, solidPredicate, airPredicate, solidVelocity, fluidDensity, coefficients, rhs);

    const Projection::IPressureSolverPtr solver = std::make_shared<SolverType>(pressure, fluidPredicate, coefficients, rhs, tolerance, maxIterationsCount);

    return std::make_shared<PressureMovementType>(velocity, pressure, fluidPredicate, solidPredicate, solidVelocity, fluidDensity,
        coefficientCalculator, solver);
}

}

AquariumFluidSystem::AquariumFluidSystem(
    const FluidSdfSpace& fluidSdf,
    const FluidSimulations::ISimulatorPtr& simulator)
    : m_fluidSdf(fluidSdf)
    , m_simulator(simulator)
{
}

std::shared_ptr<AquariumFluidSystem> AquariumFluidSystem::build(const int iRes, const int jRes, const int kRes, 
    const FluidSimulations::FloatType gravitationAcceleration, const FloatSpaceFunctor& sdf, 
    const FluidSimulations::FloatType timeInterval)
{
    using namespace FluidSimulations;
    using namespace FluidSimulations::SignedDistanceField;
    using namespace FluidSimulations::Advection;

    const FloatType solidOffset = 2.4f;

    const auto fluidSdfGrid = buildGrid(iRes, jRes, kRes, sdf);
    const auto fluidSdfInternalSpace = LinearFloatsGridApproximator(fluidSdfGrid);
    const auto fluidSdfSpace = SignedDistanceGridApproximatorExtension<LinearFloatsGridApproximator>::wrapSignedDistance(
        fluidSdfInternalSpace, fluidSdfGrid->res(), solidOffset);

    const auto fluidInteriorPredicate = InteriorPredicate<FluidSdfSpace>(fluidSdfSpace);
    const auto nearesSurfacePointCalculator = NearestSurfacePointCalculator<FluidSdfSpace>(fluidSdfSpace);

    const auto velocityGrid = bluildVelocityGrid(fluidSdfGrid->res());
    const auto internalVelocitySpace = InternalVelocitySpace(
        velocityGrid, fluidInteriorPredicate, nearesSurfacePointCalculator);
    const auto velocitySpace = borderVelocity<InternalVelocitySpace>(internalVelocitySpace, velocityGrid->res(), solidOffset);

    const auto lagrangeTracker = RungeKuttaLagrangeTracker<VelocitySpace>(velocitySpace);

    const IMovementPtr fluidSdfAdvector = std::make_shared<LagrangeFieldAdvector<FluidSdfSpace, RungeKuttaLagrangeTracker<VelocitySpace>>>(
        fluidSdfGrid, fluidSdfSpace, lagrangeTracker);

    const IMovementPtr fluidSdfMovement = std::make_shared<SignedDistanceField::SignedDistanceMovement>(
        fluidSdfAdvector, std::make_shared<SignedDistanceField::SignedDistanceSweeper<FluidSdfSpace>>(fluidSdfGrid, fluidSdfSpace));

    // TODO VS warning c4503
    const IMovementPtr velocityAdvector = std::make_shared<LagrangeVelocityAdvector<VelocitySpace, RungeKuttaLagrangeTracker<VelocitySpace>>>(
        velocityGrid, velocitySpace, lagrangeTracker);

    const IMovementPtr gravitaionAdvector = std::make_shared<ConstantSpaceForceAccelerator>(velocityGrid, 
        std::make_shared<FloatsThreeDVector>(0, -gravitationAcceleration, 0));

    const IMovementPtr pressureAdvector = buildPressureImposer(fluidSdfGrid->res(), fluidSdfSpace, velocityGrid, solidOffset);

    std::vector<IMovementPtr> movements;
    movements.push_back(fluidSdfMovement);
    movements.push_back(velocityAdvector);
    movements.push_back(gravitaionAdvector);
    movements.push_back(pressureAdvector);

    const Machinery::ITimeSuggesterConstPtr cflTimeSuggester = std::make_shared<Machinery::CflConditionTimeSuggester>(velocityGrid, gravitationAcceleration);

    const ISimulatorPtr simulator = std::make_shared<Machinery::ConstantTimeSimulator>(cflTimeSuggester, movements, timeInterval);

    return (std::shared_ptr<AquariumFluidSystem>) new AquariumFluidSystem(fluidSdfSpace, simulator);
}

const FluidSdfSpace& AquariumFluidSystem::fluidSdf() const
{
    return m_fluidSdf;
}

const FluidSimulations::ISimulatorPtr AquariumFluidSystem::simulator() const
{
    return m_simulator;
}

}