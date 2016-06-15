#pragma once

#include "AirPredicate.hpp"
#include "ConstantSpace.hpp"
#include "ScalarVectorOperations.hpp"
#include "GradientCalculator.hpp"
#include "GridOperations.hpp"
#include "IMovement.hpp"
#include "ISimulator.hpp"
#include "LinearFloatsGridApproximator.hpp"
#include "LinearObjectVelocityGridApproximator.hpp"
#include "RigidGridBoundary.hpp"
#include "RigidGridBoundaryPredicate.hpp"
#include "VelocitySpaceRigidBoundary.hpp"

#include "Advection\ConstantSpaceForceAccelerator.hpp"
#include "Advection\LagrangeFieldAdvector.hpp"
#include "Advection\LagrangeVelocityAdvector.hpp"
#include "Advection\RungeKuttaLagrangeTracker.hpp"

#include "Machinery\CflConditionTimeSuggester.hpp"
#include "Machinery\ConstantTimeSimulator.hpp"
#include "Machinery\SuggestedTimeSimulator.hpp"

#include "SignedDistanceField\InteriorPredicate.hpp"
#include "SignedDistanceField\NearestSurfacePointCalculator.hpp"
#include "SignedDistanceField\SignedDistanceGridApproximatorExtension.hpp"
#include "SignedDistanceField\SignedDistanceMovement.hpp"
#include "SignedDistanceField\SignedDistanceSweeper.hpp"

#include "Projection\GridAllignedPressureSolvePreparator.hpp"
#include "Projection\MiccgZeroSolver.hpp"
#include "Projection\VelocityProjectionMovement.hpp"