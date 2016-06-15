#pragma once

#include "Grid.hpp"
#include "ScalarVectorDefinitions.hpp"

#include <memory>

namespace FluidSimulations
{

typedef Grid<FloatType> FloatsGrid;
typedef std::shared_ptr<FloatsGrid> FloatsGridPtr;
typedef std::shared_ptr<const FloatsGrid> FloatsGridConstPtr;

typedef Grid<FloatsThreeDVector> FloatsThreeDVectorGrid;
typedef std::shared_ptr<FloatsThreeDVectorGrid> FloatsThreeDVectorGridPtr;
typedef std::shared_ptr<const FloatsThreeDVectorGrid> FloatsThreeDVectorGridConstPtr;

typedef Grid<FloatsFourDVector> FloatsFourDVectorGrid;
typedef std::shared_ptr<FloatsFourDVectorGrid> FloatsFourDVectorGridPtr;
typedef std::shared_ptr<const FloatsFourDVectorGrid> FloatsFourDVectorGridConstPtr;

typedef Grid<bool> PredicateGrid;
typedef std::shared_ptr<PredicateGrid> PredicateGridPtr;
typedef std::shared_ptr<const PredicateGrid> PredicateGridConstPtr;

}