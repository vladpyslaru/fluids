cmake_minimum_required(VERSION 2.8) 

project (FluidSimulationsDemo)

file(GLOB SOURCES
	*.h
	*.cpp
)
source_group(FluidSimulationsDemo FILES ${SOURCES})

add_executable(FluidSimulationsDemo ${SOURCES})

include_directories(../)

set(EIGEN_INCLUDE_DIR "../eigen" 
	CACHE PATH "Where is the include directory of eigen located")
include_directories(${EIGEN_INCLUDE_DIR})

set(VOLPLAY_INCLUDE_DIR "../volplay" 
	CACHE PATH "Where is the include directory of volplay located")
include_directories(${VOLPLAY_INCLUDE_DIR})

find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})
link_libraries(${OpenCV_LIBS})
add_definitions(-DVOLPLAY_WITH_OPENCV)