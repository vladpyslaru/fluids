#pragma once

#include "FluidSimulationsDemoDefinitions.hpp"
#include "VolplaySdfRenderer.hpp"

#include <opencv2\opencv.hpp>
#include <string>

namespace FluidSimulationsDemo
{

class FluidRendererVideoWriter
{
public:
    static std::shared_ptr<FluidRendererVideoWriter> build(
        const int frameRate, 
        const int timeSeconds, 
        const int codec,
        const int frameWidth, 
        const int framHeigth, 
        const std::string& path, 
        const FluidSimulations::ISimulatorPtr& fluidSystem,
        const VolplaySdfRendererConstPtr& volplayRenderer);

    FluidRendererVideoWriter(const FluidRendererVideoWriter&) = delete;
    FluidRendererVideoWriter& operator=(const FluidRendererVideoWriter&) = delete;

    void write();

private:
    FluidRendererVideoWriter(
        const FluidSimulations::FloatType m_timeInterval, 
        const int iterationsCount, 
        std::unique_ptr<cv::VideoWriter> videoWriter, 
        const FluidSimulations::ISimulatorPtr& fluidSystem, 
        const VolplaySdfRendererConstPtr& volplayRenderer);

private:
    const FluidSimulations::FloatType m_timeInterval;
    const int m_iterationsCount;
    const std::unique_ptr<cv::VideoWriter> m_videoWriter;
    const FluidSimulations::ISimulatorPtr m_fluidSystem;
    const VolplaySdfRendererConstPtr m_volplayRenderer;
};

typedef std::shared_ptr<FluidRendererVideoWriter> FluidRendererVideoWriterPtr;

}