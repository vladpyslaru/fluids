#pragma once

#include "../IMovement.hpp"
#include "../GridOperations.hpp"
#include "IPressureSolver.hpp"
#include "IPressureSolverPreparator.hpp"

namespace FluidSimulations
{

namespace Projection
{

// TODO separate boundary conditions

template <
    typename FluidPredicateSpaceType,
    typename SolidPredicateSpaceType,
    typename FloatsThreeDVectorSpaceType
    >
class VelocityProjectionMovement
    : public IMovement
{
public:
    VelocityProjectionMovement(
        const FloatsThreeDVectorGridPtr& velocity,
        const FloatsGridConstPtr& pressure,
        const FluidPredicateSpaceType& fluidPredicate,
        const SolidPredicateSpaceType& solidPredicate,
        const FloatsThreeDVectorSpaceType& solidVelocity,
        const FloatType fluidDensity,
        const IPressureSolverPreparatorPtr& pressureSolverPreparator,
        const IPressureSolverPtr& pressureSolver)
        : m_velocity(velocity)
        , m_pressure(pressure)
        , m_fluidPredicate(fluidPredicate)
        , m_solidPredicate(solidPredicate)
        , m_solidVelocity(solidVelocity)
        , m_fluidDensity(fluidDensity)
        , m_pressureSolverPreparator(pressureSolverPreparator)
        , m_pressureSolver(pressureSolver)
    {}

    VelocityProjectionMovement(const VelocityProjectionMovement&) = delete;
    VelocityProjectionMovement& operator=(const VelocityProjectionMovement&) = delete;

    virtual void move(const FloatType timeInterval) override
    {
        m_pressureSolverPreparator->prepare(timeInterval);
        
        m_pressureSolver->solve();

        updateVelocities(timeInterval);
    }

private:
    void updateVelocities(const FloatType timeInterval)
    {
        const FloatType scale = timeInterval / m_fluidDensity;

        for (IntegerType i = 0; i < m_pressure->iRes(); ++i)
        {
            for (IntegerType j = 0; j < m_pressure->jRes(); ++j)
            {
                for (IntegerType k = 0; k < m_pressure->kRes(); ++k)
                {
                    if (m_fluidPredicate.at(i, j, k))
                    {
                        const FloatType pValue = scale * m_pressure->at(i, j, k);

                        m_velocity->at(i, j, k).x -= pValue;
                        m_velocity->at(i + 1, j, k).x += pValue;

                        m_velocity->at(i, j, k).y -= pValue;
                        m_velocity->at(i, j + 1, k).y += pValue;

                        m_velocity->at(i, j, k).z -= pValue;
                        m_velocity->at(i, j, k + 1).z += pValue;
                    }

                    if (m_solidPredicate.at(i, j, k))
                    {
                        m_velocity->at(i, j, k) = m_solidVelocity.at(i, j, k);

                        m_velocity->at(i + 1, j, k).x = m_solidVelocity.at(i + 1, j, k).x;
                        m_velocity->at(i, j + 1, k).y = m_solidVelocity.at(i, j + 1, k).y;
                        m_velocity->at(i, j, k + 1).z = m_solidVelocity.at(i, j, k + 1).z;
                    }
                }
            }
        }
    }

private:
    const FloatsThreeDVectorGridPtr m_velocity;
    const FloatsGridConstPtr m_pressure;

    const FluidPredicateSpaceType m_fluidPredicate;
    const SolidPredicateSpaceType m_solidPredicate;
    const FloatsThreeDVectorSpaceType m_solidVelocity;

    const FloatType m_fluidDensity;

    const IPressureSolverPreparatorPtr m_pressureSolverPreparator;
    const IPressureSolverPtr m_pressureSolver;

};

}

}
