#include "SdfFunctions.hpp"

namespace FluidSimulationsDemo
{

using namespace FluidSimulations;

FloatSpaceFunctor waterBallSdf(const FluidSimulations::FloatsThreeDVector& center, 
    const FluidSimulations::FloatType& radius)
{
    const FloatSpaceFunctor functor = [center, radius](const FloatType x, const FloatType y, const FloatType z) -> FloatType
        {
            FluidSimulations::FloatsThreeDVector position(x, y, z);

            FluidSimulations::sumIn(FluidSimulations::multiplyIn<FloatType>(position, -1), center);

            return norm(position) - radius;
        };

    return functor;
}

}