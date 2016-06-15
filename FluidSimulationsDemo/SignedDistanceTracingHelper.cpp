#include "SignedDistanceTracingHelper.hpp"

using namespace volplay;

namespace FluidSimulationsDemo
{

SignedDistanceTracingHelper::SignedDistanceTracingHelper(const FluidSdfSpace& sdf)
    : m_sdf(sdf)
{
}

volplay::SDFResult SignedDistanceTracingHelper::fullEval(const volplay::Vector& p) const
{
    SDFResult result;

    result.node = this;

    result.sdf = static_cast<volplay::Scalar>(m_sdf.at(p[0], p[1], p[2]));

    if (result.sdf < 0)
    {
        result.sdf = 0;
    }

    return result;
}

}