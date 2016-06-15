#pragma once

#include "../GridDefinitions.hpp"
#include "IPressureSolver.hpp"

namespace FluidSimulations
{

namespace Projection
{

template <typename PredicateSpaceType>
class MiccgZeroSolver
    : public IPressureSolver
{
public:
    MiccgZeroSolver(
        const FloatsGridPtr& pressure, 
        const PredicateSpaceType& predicate,
        const FloatsFourDVectorGridConstPtr& coefficients, 
        const FloatsGridConstPtr& rhs,
        const FloatType tolerance, 
        const IntegerType maxIterations)
        : m_pressure(pressure)
        , m_predicate(predicate)
        , m_coefficients(coefficients)
        , m_rhs(rhs)
        , m_tolerance(tolerance)
        , m_maxIterations(maxIterations)
        , m_search(std::make_shared<FloatsGrid>(pressure->iRes(), pressure->jRes(), pressure->kRes(), static_cast<FloatType>(0)))
        , m_auxiliary(std::make_shared<FloatsGrid>(pressure->iRes(), pressure->jRes(), pressure->kRes(), static_cast<FloatType>(0)))
        , m_residual(std::make_shared<FloatsGrid>(pressure->iRes(), pressure->jRes(), pressure->kRes(), static_cast<FloatType>(0)))
        , m_preconditionerBuffer(std::make_shared<FloatsGrid>(pressure->iRes(), pressure->jRes(), pressure->kRes(), static_cast<FloatType>(0)))
        , m_factorizationSolveBuffer(std::make_shared<FloatsGrid>(pressure->iRes(), pressure->jRes(), pressure->kRes(), static_cast<FloatType>(0)))
    {
    }

    MiccgZeroSolver(const MiccgZeroSolver&) = delete;
    MiccgZeroSolver& operator=(const MiccgZeroSolver&) = delete;

    virtual void solve() override
    {
        setValues<FloatType>(m_pressure, 0);
        setValues<FloatType>(m_auxiliary, 0);
        setValues<FloatType>(m_preconditionerBuffer, 0);
        setValues<FloatType>(m_factorizationSolveBuffer, 0);

        const FloatType stopRate = calculateStopRate(m_rhs, m_predicate);
        if (stopRate <= m_tolerance)
        {
            return;
        }

        copyIn<FloatType>(m_rhs, m_residual);

        applyPreconditioner();

        copyIn<FloatType>(m_auxiliary, m_search);

        FloatType sigma = multiply(m_auxiliary, m_residual, m_predicate);

        for (IntegerType iteration = 0; iteration < m_maxIterations; ++iteration)
        {
            applyMatrix(m_auxiliary, m_coefficients, m_search, m_predicate);

            const FloatType alpha = sigma / multiply(m_auxiliary, m_search, m_predicate);

            sumIn(m_pressure, 1, m_pressure, alpha, m_search, m_predicate);

            sumIn(m_residual, 1, m_residual, -alpha, m_auxiliary, m_predicate);

            const FloatType stopRate = calculateStopRate(m_residual, m_predicate);

            if (stopRate <= m_tolerance)
            {
                return;
            }

            applyPreconditioner();

            const FloatType sigmaNew = multiply(m_auxiliary, m_residual, m_predicate);

            const FloatType betta = sigmaNew / sigma;

            sumIn(m_search, 1, m_auxiliary, betta, m_search, m_predicate);

            sigma = sigmaNew;
        }
    }

private:
    static inline FloatType calculateStopRate(
        const FloatsGridConstPtr& targetGrid, 
        const PredicateSpaceType& predicate)
    {
        if (targetGrid->iRes() == 0 || targetGrid->jRes() == 0 || targetGrid->kRes() == 0)
        {
            return 0;
        }

        FloatType maxAbsValue = targetGrid->at(0, 0, 0);

        for (IntegerType i = 0; i < targetGrid->iRes(); ++i)
        {
            for (IntegerType j = 0; j < targetGrid->jRes(); ++j)
            {
                for (IntegerType k = 0; k < targetGrid->kRes(); ++k)
                {
                    if (predicate.at(i, j, k))
                    {
                        const FloatType currentAbsValue = abs(targetGrid->at(i, j, k));

                        if (currentAbsValue > maxAbsValue)
                        {
                            maxAbsValue = currentAbsValue;
                        }
                    }
                }
            }
        }

        return maxAbsValue;
    }

    static inline FloatType multiply(
        const FloatsGridConstPtr& left, 
        const FloatsGridConstPtr& right, 
        const PredicateSpaceType& predicate)
    {
        FloatType result = 0;

        for (IntegerType i = 0; i < left->iRes(); ++i)
        {
            for (IntegerType j = 0; j < left->jRes(); ++j)
            {
                for (IntegerType k = 0; k < left->kRes(); ++k)
                {
                    if (predicate.at(i, j, k))
                    {
                        result += left->at(i, j, k) * right->at(i, j, k);
                    }
                }
            }
        }

        return result;
    }

    static inline void sumIn(
        const FloatsGridPtr& target, 
        const FloatType factorLeft, 
        const FloatsGridConstPtr& sourceLeft,
        const FloatType factorRight, 
        const FloatsGridConstPtr& sourceRight, 
        const PredicateSpaceType& predicate)
    {
        for (IntegerType i = 0; i < target->iRes(); ++i)
        {
            for (IntegerType j = 0; j < target->jRes(); ++j)
            {
                for (IntegerType k = 0; k < target->kRes(); ++k)
                {
                    if (predicate.at(i, j, k))
                    {
                        target->at(i, j, k) = factorLeft * sourceLeft->at(i, j, k) + factorRight * sourceRight->at(i, j, k);
                    }
                }
            }
        }
    }

    static void applyMatrix(
        const FloatsGridPtr& target, 
        const FloatsFourDVectorGridConstPtr& coefficients,
        const FloatsGridConstPtr& source, 
        const PredicateSpaceType& predicate)
    {
        for (IntegerType i = 0; i < target->iRes(); ++i)
        {
            for (IntegerType j = 0; j < target->jRes(); ++j)
            {
                for (IntegerType k = 0; k < target->kRes(); ++k)
                {
                    if (!predicate.at(i, j, k))
                    {
                        continue;
                    }

                    const FloatType spi = i < source->iRes() - 1 && predicate.at(i + 1, j, k)
                        ? source->at(i + 1, j, k) * coefficients->at(i, j, k).y
                        : 0;

                    const FloatType spj = j < source->jRes() - 1 && predicate.at(i, j + 1, k)
                        ? source->at(i, j + 1, k) * coefficients->at(i, j, k).z
                        : 0;

                    const FloatType spk = k < source->kRes() - 1 && predicate.at(i, j, k + 1)
                        ? source->at(i, j, k + 1) * coefficients->at(i, j, k).w
                        : 0;

                    const FloatType smi = i > 0 && predicate.at(i - 1, j, k)
                        ? source->at(i - 1, j, k) * coefficients->at(i - 1, j, k).y
                        : 0;

                    const FloatType smj = j > 0 && predicate.at(i, j - 1, k)
                        ? source->at(i, j - 1, k) * coefficients->at(i, j - 1, k).z
                        : 0;

                    const FloatType smk = k > 0 && predicate.at(i, j, k - 1)
                        ? source->at(i, j, k - 1) * coefficients->at(i, j, k - 1).w
                        : 0;

                    target->at(i, j, k) = source->at(i, j, k) * coefficients->at(i, j, k).x
                        + spi + spj + spk + smi + smj + smk;
                }
            }
        }
    }

    void applyPreconditioner()
    {
        calculatePrecondition();
        performFactorizationSolve();
    }

    void calculatePrecondition()
    {
        constexpr FloatType tau = 0.97f;
        constexpr FloatType sigma = 0.25f;

        for (IntegerType i = 0; i < m_preconditionerBuffer->iRes(); ++i)
        {
            for (IntegerType j = 0; j < m_preconditionerBuffer->jRes(); ++j)
            {
                for (IntegerType k = 0; k < m_preconditionerBuffer->kRes(); ++k)
                {
                    if (!m_predicate.at(i, j, k))
                    {
                        continue;
                    }

                    const FloatType aValue = m_coefficients->at(i, j, k).x;

                    const bool fmi = (i >= 1) && m_predicate.at(i - 1, j, k);
                    const FloatType apimi = !fmi ? 0 : m_coefficients->at(i - 1, j, k).y;
                    const FloatType apjmi = !fmi ? 0 : m_coefficients->at(i - 1, j, k).z;
                    const FloatType apkmi = !fmi ? 0 : m_coefficients->at(i - 1, j, k).w;

                    const bool fmj = (j >= 1) && m_predicate.at(i, j - 1, k);
                    const FloatType apimj = !fmj ? 0 : m_coefficients->at(i, j - 1, k).y;
                    const FloatType apjmj = !fmj ? 0 : m_coefficients->at(i, j - 1, k).z;
                    const FloatType apkmj = !fmj ? 0 : m_coefficients->at(i, j - 1, k).w;

                    const bool fmk = (k >= 1) && m_predicate.at(i, j, k - 1);
                    const FloatType apimk = !fmk ? 0 : m_coefficients->at(i, j, k - 1).y;
                    const FloatType apjmk = !fmk ? 0 : m_coefficients->at(i, j, k - 1).z;
                    const FloatType apkmk = !fmk ? 0 : m_coefficients->at(i, j, k - 1).w;

                    const FloatType pmi = !fmi ? 0 : m_preconditionerBuffer->at(i - 1, j, k);
                    const FloatType pmj = !fmj ? 0 : m_preconditionerBuffer->at(i, j - 1, k);
                    const FloatType pmk = !fmk ? 0 : m_preconditionerBuffer->at(i, j, k - 1);

                    const FloatType tpi = apimi * pmi;
                    const FloatType tpj = apjmj * pmj;
                    const FloatType tpk = apkmk * pmk;

                    const FloatType preTauValue = apimi * (apjmi + apkmi) * pmi * pmi
                        + apjmj * (apimj + apkmj) * pmj * pmj + apkmk * (apimk + apjmk) * pmk * pmk;

                    const FloatType eValue = aValue - tpi * tpi - tpj * tpj - tpk * tpk - tau * preTauValue;

                    const FloatType dValue = eValue < sigma * aValue ? aValue : eValue;

                    m_preconditionerBuffer->at(i, j, k) = 1 / sqrt(dValue);
                }
            }
        }
    }

    void performFactorizationSolve()
    {
        for (IntegerType i = 0; i < m_factorizationSolveBuffer->iRes(); ++i)
        {
            for (IntegerType j = 0; j < m_factorizationSolveBuffer->jRes(); ++j)
            {
                for (IntegerType k = 0; k < m_factorizationSolveBuffer->kRes(); ++k)
                {
                    if (!m_predicate.at(i, j, k))
                    {
                        continue;
                    }

                    const bool fmi = (i >= 1) && m_predicate.at(i - 1, j, k);
                    const FloatType ft = !fmi ? 0 : m_coefficients->at(i - 1, j, k).y * m_preconditionerBuffer->at(i - 1, j, k) * m_factorizationSolveBuffer->at(i - 1, j, k);

                    const bool fmj = (j >= 1) && m_predicate.at(i, j - 1, k);
                    const FloatType st = !fmj ? 0 : m_coefficients->at(i, j - 1, k).z * m_preconditionerBuffer->at(i, j - 1, k) * m_factorizationSolveBuffer->at(i, j - 1, k);

                    const bool fmk = (k >= 1) && m_predicate.at(i, j, k - 1);
                    const FloatType tt = !fmk ? 0 : m_coefficients->at(i, j, k - 1).w * m_preconditionerBuffer->at(i, j, k - 1) * m_factorizationSolveBuffer->at(i, j, k - 1);

                    const FloatType tValue = m_residual->at(i, j, k) - ft - st - tt;

                    m_factorizationSolveBuffer->at(i, j, k) = tValue * m_preconditionerBuffer->at(i, j, k);
                }
            }
        }

        for (IntegerType i = m_auxiliary->iRes() - 1; i >= 0; --i)
        {
            for (IntegerType j = m_auxiliary->jRes() - 1; j >= 0; --j)
            {
                for (IntegerType k = m_auxiliary->kRes() - 1; k >= 0; --k)
                {
                    if (!m_predicate.at(i, j, k))
                    {
                        continue;
                    }

                    const bool fmi = (i < m_auxiliary->iRes() - 1) && m_predicate.at(i + 1, j, k);
                    const FloatType ft = !fmi ? 0 : m_coefficients->at(i, j, k).y * m_preconditionerBuffer->at(i, j, k) * m_auxiliary->at(i + 1, j, k);

                    const bool fmj = (j < m_auxiliary->jRes() - 1) && m_predicate.at(i, j + 1, k);
                    const FloatType st = !fmj ? 0 : m_coefficients->at(i, j, k).z * m_preconditionerBuffer->at(i, j, k) * m_auxiliary->at(i, j + 1, k);

                    const bool fmk = (k < m_auxiliary->kRes() - 1) && m_predicate.at(i, j, k + 1);
                    const FloatType tt = !fmk ? 0 : m_coefficients->at(i, j, k).w * m_preconditionerBuffer->at(i, j, k) * m_auxiliary->at(i, j, k + 1);

                    const FloatType tValue = m_factorizationSolveBuffer->at(i, j, k) - ft - st - tt;

                    m_auxiliary->at(i, j, k) = tValue * m_preconditionerBuffer->at(i, j, k);
                }
            }
        }
    }

private:
    const FloatsGridPtr m_pressure;

    const PredicateSpaceType m_predicate;

    const FloatsFourDVectorGridConstPtr m_coefficients;
    const FloatsGridConstPtr m_rhs;

    const FloatType m_tolerance;
    const IntegerType m_maxIterations;

    const FloatsGridPtr m_search;
    const FloatsGridPtr m_auxiliary;
    const FloatsGridPtr m_residual;
    const FloatsGridPtr m_preconditionerBuffer;
    const FloatsGridPtr m_factorizationSolveBuffer;
};

}

}