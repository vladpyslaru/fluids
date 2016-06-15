#include "AquariumFluidSystem.hpp"
#include "SdfFunctions.hpp"
#include "DemoVideoWriter.hpp"

#include <iostream>
#include <utility>

namespace FluidSimulationsDemo
{

namespace
{

const int c_frameWidth = 800;
const int c_frameHeigth = 600;

const FluidSimulations::FloatsThreeDVector c_fluidColor(0, 0, 1);

const int c_resolution = 150;

const float c_gravitation = 0.196f;

std::pair<FluidSimulations::ISimulatorPtr, VolplaySdfRendererConstPtr> buildWaterBallMachinery(
    const FluidSimulations::FloatType timeInterval)
{
    const FluidSimulations::FloatType& res = static_cast<FluidSimulations::FloatType>(c_resolution);
    const FluidSimulations::FloatType halfRes = c_resolution * 0.5f;

    const FluidSimulations::FloatsThreeDVector center(halfRes, halfRes, halfRes);
    const FluidSimulations::FloatType radius = c_resolution * 0.2;

    const FluidSimulations::FloatType gravitation = c_gravitation * c_resolution;

    const std::shared_ptr<AquariumFluidSystem> fluidSystem = AquariumFluidSystem::build(c_resolution, c_resolution, c_resolution,
        gravitation, waterBallSdf(center, radius), timeInterval);

    const FluidSimulations::FloatsThreeDVector viewPoint(-3 * res, 2 * res, halfRes);
    const FluidSimulations::FloatsThreeDVector viewPointCenter(halfRes, 0, halfRes);
    const FluidSimulations::FloatsThreeDVector up(0, 1, 0);

    const FluidSimulations::FloatsThreeDVector lightPosition(halfRes, 3 * res, halfRes);
    const FluidSimulations::FloatType lightIntensity = lightPosition.y * 10;

    const VolplaySdfRendererConstPtr renderer = std::make_shared<VolplaySdfRenderer>(c_frameWidth, c_frameHeigth,
        FluidSimulations::IntegersThreeDVector(c_resolution, c_resolution, c_resolution), fluidSystem->fluidSdf(),
        viewPoint, viewPointCenter, up, lightPosition, lightIntensity, c_fluidColor);

    return std::make_pair(fluidSystem->simulator(), renderer);
}

void writeVideo(const int secondsCount)
{
    const std::string videoDumpPath = "./simulation";

    const std::string extension = ".avi";

    const int codecOptions = -1;
    const int fullFramesMode = 0;
    const int wmvCodec = cv::VideoWriter::fourcc('W', 'M', 'V', '3');
    const int xvidCodec = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');

    const int codec = xvidCodec;

    const int framRate = 25;

    const auto machinery = buildWaterBallMachinery(1.0f / framRate);

    const FluidRendererVideoWriterPtr videoWriter = FluidRendererVideoWriter::build(framRate, secondsCount, codec,
        c_frameWidth, c_frameHeigth, videoDumpPath + extension, machinery.first, machinery.second);

    if (!videoWriter)
    {
        std::cout << "Failed to open vide file" << std::endl;
        return;
    }

    videoWriter->write();
}

}

}

int main()
{
    FluidSimulationsDemo::writeVideo(10);

    return 0;
}

