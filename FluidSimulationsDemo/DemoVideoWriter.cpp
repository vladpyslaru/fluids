#include "DemoVideoWriter.hpp"

#include <opencv2/opencv.hpp>
#include <iostream>

namespace FluidSimulationsDemo
{

namespace
{

inline void addFrameInfo(const int frameIndex, cv::Mat image)
{
    const int tX = image.rows / 25;
    const int tY = image.cols / 25;

    cv::putText(image, std::to_string(frameIndex), cv::Point(tX, tY), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255));
}

}

std::shared_ptr<FluidRendererVideoWriter> FluidRendererVideoWriter::build(
    const int frameRate, 
    const int timeSeconds, 
    const int codec, 
    const int frameWidth, 
    const int framHeigth, 
    const std::string& path, 
    const FluidSimulations::ISimulatorPtr& fluidSystem,
    const VolplaySdfRendererConstPtr& volplayRenderer)
{
    std::unique_ptr<cv::VideoWriter> videoBuilder(new cv::VideoWriter());

    videoBuilder->open(cv::String(path), codec, frameRate, cv::Size(frameWidth, framHeigth));
    if (!videoBuilder->isOpened())
    {
        return FluidRendererVideoWriterPtr();
    }
    
    const int totalFramesCount = frameRate * timeSeconds;
    const FluidSimulations::FloatType timeStep = FluidSimulations::FloatType(1) / static_cast<FluidSimulations::FloatType>(frameRate);

    return (FluidRendererVideoWriterPtr) new FluidRendererVideoWriter(timeStep, totalFramesCount, std::move(videoBuilder), fluidSystem, volplayRenderer);
}

FluidRendererVideoWriter::FluidRendererVideoWriter(
    const FluidSimulations::FloatType timeInterval, 
    const int iterationsCount, 
    std::unique_ptr<cv::VideoWriter> videoWriter, 
    const FluidSimulations::ISimulatorPtr& fluidSystem, 
    const VolplaySdfRendererConstPtr& volplayRenderer)
    : m_timeInterval(timeInterval)
    , m_iterationsCount(iterationsCount)
    , m_videoWriter(std::move(videoWriter))
    , m_fluidSystem(fluidSystem)
    , m_volplayRenderer(volplayRenderer)
{
}

void FluidRendererVideoWriter::write()
{
    const double progressDenominator = static_cast<double>(1) / m_iterationsCount;

    for (int c = 0; c < m_iterationsCount; ++c)
    {
        m_fluidSystem->step();

        cv::Mat frame = m_volplayRenderer->render();
        addFrameInfo(c, frame);

        m_videoWriter->write(frame);

        const int progress = static_cast<int>(std::round(c * progressDenominator * 100));
        std::cout << "[ VideoWriter ] Frame " << c << "; Progress " << progress << "%" << std::endl;
    }
}

}