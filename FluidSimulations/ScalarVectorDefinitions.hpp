#pragma once

#include "ScalarTypes.hpp"
#include "ScalarThreeDVector.hpp"
#include "ScalarFourdDVector.hpp"

namespace FluidSimulations
{

typedef ScalarThreeDVector<FloatType> FloatsThreeDVector;
typedef ScalarThreeDVector<IntegerType> IntegersThreeDVector;

typedef ScalarFourdDVector<FloatType> FloatsFourDVector;
typedef ScalarFourdDVector<IntegerType> IntegersFourDVector;

}
