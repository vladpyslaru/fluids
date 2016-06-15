#pragma once

#include "../GridOperations.hpp"
#include "IPressureSolverPreparator.hpp"

namespace FluidSimulations
{

namespace Projection
{

template 
<
    typename FluidPredicateSpaceType, 
    typename SolidPredicateSpaceType, 
    typename AirPredicateSpaceType, 
    typename FloatsThreeDVectorSpaceType
>
class GridAllignedPressureSolvePreparator
    : public IPressureSolverPreparator
{
public:
    GridAllignedPressureSolvePreparator(
        const IntegersThreeDVector& resolution, 
        const FloatsThreeDVectorGridConstPtr& velocity,
        const FluidPredicateSpaceType& fluidPredicate, 
        const SolidPredicateSpaceType& solidPredicate, 
        const AirPredicateSpaceType& airPredicate,
        const FloatsThreeDVectorSpaceType& solidVelocity, 
        const FloatType fluidDensity,
        const FloatsFourDVectorGridPtr& coefficients,
        const FloatsGridPtr& rhs)
        : m_velocity(velocity)
        , m_fluidPredicate(fluidPredicate)
        , m_solidPredicate(solidPredicate)
        , m_airPredicate(airPredicate)
        , m_solidVelocity(solidVelocity)
        , m_fluidDensity(fluidDensity)
        , m_coefficients(coefficients)
        , m_rhs(rhs)
    {}

    GridAllignedPressureSolvePreparator(const GridAllignedPressureSolvePreparator&) = delete;
    GridAllignedPressureSolvePreparator& operator=(const GridAllignedPressureSolvePreparator&) = delete;

    virtual void prepare(const FloatType timeInterval) override
    {
        calculateRightHandSide();
        calculateCoefficients(timeInterval);
    }

private:
    void calculateRightHandSide()
    {
        setValues<FloatType>(m_rhs, 0);

        for (IntegerType i = 0; i < m_rhs->iRes(); ++i)
        {
            for (IntegerType j = 0; j < m_rhs->jRes(); ++j)
            {
                for (IntegerType k = 0; k < m_rhs->kRes(); ++k)
                {
                    if (!m_fluidPredicate.at(i, j, k))
                    {
                        continue;
                    }

                    FloatType targetValue = m_velocity->at(i, j, k).x - m_velocity->at(i + 1, j, k).x
                        + m_velocity->at(i, j, k).y - m_velocity->at(i, j + 1, k).y
                        + m_velocity->at(i, j, k).z - m_velocity->at(i, j, k + 1).z;

                    if (m_solidPredicate.at(i - 1, j, k))
                    {
                        targetValue -= m_velocity->at(i, j, k).x - m_solidVelocity.at(i, j, k).x;
                    }

                    if (m_solidPredicate.at(i + 1, j, k))
                    {
                        targetValue += m_velocity->at(i + 1, j, k).x - m_solidVelocity.at(i + 1, j, k).x;
                    }

                    if (m_solidPredicate.at(i, j - 1, k))
                    {
                        targetValue -= m_velocity->at(i, j, k).y - m_solidVelocity.at(i, j, k).y;
                    }

                    if (m_solidPredicate.at(i, j + 1, k))
                    {
                        targetValue += m_velocity->at(i, j + 1, k).y - m_solidVelocity.at(i, j + 1, k).y;
                    }

                    if (m_solidPredicate.at(i, j, k - 1))
                    {
                        targetValue -= m_velocity->at(i, j, k).z - m_solidVelocity.at(i, j, k).z;
                    }

                    if (m_solidPredicate.at(i, j, k + 1))
                    {
                        targetValue += m_velocity->at(i, j, k + 1).z - m_solidVelocity.at(i, j, k + 1).z;
                    }

                    m_rhs->at(i, j, k) = targetValue;
                }
            }
        }
    }

    void calculateCoefficients(const FloatType timeInterval)
    {
        const FloatType scale = timeInterval / m_fluidDensity;

        setValues<FloatsFourDVector>(m_coefficients, FloatsFourDVector());

        for (IntegerType i = 0; i < m_coefficients->iRes(); ++i)
        {
            for (IntegerType j = 0; j < m_coefficients->jRes(); ++j)
            {
                for (IntegerType k = 0; k < m_coefficients->kRes(); ++k)
                {
                    if (!m_fluidPredicate.at(i, j, k))
                    {
                        continue;
                    }

                    if ((i < m_coefficients->iRes() - 1) && m_fluidPredicate.at(i + 1, j, k))
                    {
                        m_coefficients->at(i, j, k).x += scale;
                        m_coefficients->at(i + 1, j, k).x += scale;
                        m_coefficients->at(i, j, k).y = -scale;
                    }

                    if (m_airPredicate.at(i + 1, j, k))
                    {
                        m_coefficients->at(i, j, k).x += scale;
                    }

                    if (m_airPredicate.at(i - 1, j, k))
                    {
                        m_coefficients->at(i, j, k).x += scale;
                    }

                    if ((j < m_coefficients->jRes() - 1) && m_fluidPredicate.at(i, j + 1, k))
                    {
                        m_coefficients->at(i, j, k).x += scale;
                        m_coefficients->at(i, j + 1, k).x += scale;
                        m_coefficients->at(i, j, k).z = -scale;
                    }

                    if (m_airPredicate.at(i, j + 1, k))
                    {
                        m_coefficients->at(i, j, k).x += scale;
                    }

                    if (m_airPredicate.at(i, j - 1, k))
                    {
                        m_coefficients->at(i, j, k).x += scale;
                    }

                    if ((k < m_coefficients->kRes() - 1) && m_fluidPredicate.at(i, j, k + 1))
                    {
                        m_coefficients->at(i, j, k).x += scale;
                        m_coefficients->at(i, j, k + 1).x += scale;
                        m_coefficients->at(i, j, k).w = -scale;
                    }

                    if (m_airPredicate.at(i, j, k + 1))
                    {
                        m_coefficients->at(i, j, k).x += scale;
                    }

                    if (m_airPredicate.at(i, j, k - 1))
                    {
                        m_coefficients->at(i, j, k).x += scale;
                    }
                }
            }
        }
    }

private:
    const FloatsThreeDVectorGridConstPtr m_velocity;

    const FluidPredicateSpaceType m_fluidPredicate;
    const SolidPredicateSpaceType m_solidPredicate;
    const AirPredicateSpaceType m_airPredicate;
    const FloatsThreeDVectorSpaceType m_solidVelocity;
    
    const FloatType m_fluidDensity;

    const FloatsFourDVectorGridPtr m_coefficients;
    const FloatsGridPtr m_rhs;
};



}

}