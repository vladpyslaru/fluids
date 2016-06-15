#pragma once

#include "FluidSimulationsDemoDefinitions.hpp"

#include <volplay\volplay.h>
#include <opencv2\opencv.hpp>

namespace FluidSimulationsDemo
{

class VolplaySdfRenderer
{
public:
    VolplaySdfRenderer(
        const int imageWidth, 
        const int imageHeight,
        const FluidSimulations::IntegersThreeDVector& resolution,
        const FluidSdfSpace& targetSdf,
        const FluidSimulations::FloatsThreeDVector& viewPoint,
        const FluidSimulations::FloatsThreeDVector& lookPoint,
        const FluidSimulations::FloatsThreeDVector& up,
        const FluidSimulations::FloatsThreeDVector& lightPosition,
        const FluidSimulations::FloatType lightIntensity,
        const FluidSimulations::FloatsThreeDVector& rgbColor);

    const VolplaySdfRenderer(const VolplaySdfRenderer&) = delete;
    VolplaySdfRenderer& operator=(const VolplaySdfRenderer&) = delete;

    cv::Mat render() const;

private:
    const volplay::rendering::BlinnPhongImageGeneratorPtr m_imageGenerator;
    const volplay::rendering::RendererPtr m_renderer;
};

typedef std::shared_ptr<const VolplaySdfRenderer> VolplaySdfRendererConstPtr;

}
