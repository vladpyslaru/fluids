#include "VolplaySdfRenderer.hpp"
#include "SignedDistanceTracingHelper.hpp"

#include <algorithm>

using namespace volplay;
using namespace volplay::rendering;

namespace FluidSimulationsDemo
{

namespace
{

cv::Mat toBgr(const cv::Mat &rgb)
{
    cv::Mat bgr;

#if CV_VERSION_MAJOR >= 3
    cv::cvtColor(rgb, bgr, cv::COLOR_RGB2BGR);
#else
    cv::cvtColor(rgb, bgr, CV_RGB2BGR);
#endif

    return bgr;
}

inline Vector toVector(const FluidSimulations::FloatsThreeDVector& fVector)
{
    return Vector(static_cast<Scalar>(fVector.x), static_cast<Scalar>(fVector.y), static_cast<Scalar>(fVector.z));
}

inline Scalar calculateMinTraceDistance(const FluidSimulations::FloatsThreeDVector& position,
    const FluidSimulations::IntegersThreeDVector& res)
{
    if (!FluidSimulations::positionIsOutsideGrid(position, res))
    {
        return 0;
    }

    return static_cast<Scalar>(FluidSimulations::distance(
        position, FluidSimulations::cutByGrid(position, res)));
}

inline Scalar calculateMaxTraceDistance(const FluidSimulations::FloatsThreeDVector& position,
    const FluidSimulations::IntegersThreeDVector& res, const Scalar& minTraceDistance)
{
    // max length from corner to corner via cube diagonal
    const Scalar gridMaxLength = static_cast<Scalar>(std::max(res.x, std::max(res.y, res.z))) * 2;

    if (!FluidSimulations::positionIsOutsideGrid(position, res))
    {
        return gridMaxLength;
    }

    return gridMaxLength + minTraceDistance;
}

BlinnPhongImageGeneratorPtr buildImageGenerator()
{
    BlinnPhongImageGeneratorPtr phong = std::make_shared<BlinnPhongImageGenerator>();
    phong->setAntialiasingEnabled(true);

    return phong;
}

SDFNode::TraceOptions buildTraceOptions(const FluidSimulations::FloatsThreeDVector& viewPoint,
    const FluidSimulations::IntegersThreeDVector& resolution)
{
    SDFNode::TraceOptions traceOptions;
    traceOptions.maxIter = 1000;
    traceOptions.stepFact = Scalar(0.8);
    traceOptions.sdfThreshold = 0.001f;

    // TODO
    traceOptions.minT = 0;// calculateMinTraceDistance(viewPoint, resolution);
    traceOptions.maxT = 1000;// calculateMaxTraceDistance(viewPoint, resolution, traceOptions.minT);

    return traceOptions;
}

MaterialPtr buildMaterial(const FluidSimulations::FloatsThreeDVector& rgbColor)
{
    MaterialPtr material = std::make_shared<Material>();
    material->setDiffuseColor(toVector(rgbColor));
    material->setAmbientColor(material->diffuseColor() * Scalar(0.1));
    material->setSpecularColor(Vector::Ones());
    material->setSpecularHardness(Scalar(128));

    return material;
}

SDFNodePtr wrapSdfNode(const FluidSdfSpace& targetSdfGrid,
    const FluidSimulations::FloatsThreeDVector& rgbColor)
{
    // TODO
    SDFNodePtr sdfNode = std::make_shared<SignedDistanceTracingHelper>(targetSdfGrid);
    //sdfNode->setAttachment();

    SDFNodePtr scene = volplay::make()
        .join().plane().normal(volplay::Vector::UnitY()).end()
        .join().wrap().node(sdfNode).attach("Material", buildMaterial(rgbColor)).end();

    return scene;
}

std::vector<LightPtr> buildLights(const FluidSimulations::FloatsThreeDVector& lightPosition,
    const FluidSimulations::FloatType lightIntensity)
{
    return std::vector<LightPtr>(1, Light::createPointLight(toVector(lightPosition), Vector::Ones(), Vector::Ones(), Vector::Ones(), 
        static_cast<volplay::Scalar>(lightIntensity)));
}

CameraPtr buildCamera(const int imageWidth, const int imageHeight, 
    const FluidSimulations::FloatsThreeDVector& viewPoint,
    const FluidSimulations::FloatsThreeDVector& lookPoint,
    const FluidSimulations::FloatsThreeDVector& up)
{
    CameraPtr camera = std::make_shared<Camera>();
    camera->setCameraToImage(imageHeight, imageWidth, Scalar(0.4));
    camera->setCameraToWorldAsLookAt(toVector(viewPoint), toVector(lookPoint), toVector(up));

    return camera;
}

RendererPtr buildRenderer(const int imageWidth, const int imageHeight,
    const FluidSimulations::IntegersThreeDVector& resolution,
    const FluidSdfSpace& targetSdf,
    const FluidSimulations::FloatsThreeDVector& viewPoint,
    const FluidSimulations::FloatsThreeDVector& lookPoint,
    const FluidSimulations::FloatsThreeDVector& up,
    const FluidSimulations::FloatsThreeDVector& lightPosition,
    const FluidSimulations::FloatType lightIntensity,
    const FluidSimulations::FloatsThreeDVector& rgbColor,
    const ImageGeneratorPtr& generator)
{
    const RendererPtr renderer = std::make_shared<Renderer>();

    renderer->setImageResolution(imageHeight, imageWidth);

    renderer->setPrimaryTraceOptions(buildTraceOptions(viewPoint, resolution));

    renderer->setScene(wrapSdfNode(targetSdf, rgbColor));

    renderer->setCamera(buildCamera(imageWidth, imageHeight, viewPoint, lookPoint, up));

    renderer->setLights(buildLights(lightPosition, lightIntensity));

    renderer->addImageGenerator(generator);

    return renderer;
}

}

VolplaySdfRenderer::VolplaySdfRenderer(const int imageWidth, const int imageHeight,
    const FluidSimulations::IntegersThreeDVector& resolution,
    const FluidSdfSpace& targetSdf,
    const FluidSimulations::FloatsThreeDVector& viewPoint, 
    const FluidSimulations::FloatsThreeDVector& lookPoint,
    const FluidSimulations::FloatsThreeDVector& up,
    const FluidSimulations::FloatsThreeDVector& lightPosition,
    const FluidSimulations::FloatType lightIntensity,
    const FluidSimulations::FloatsThreeDVector& rgbColor)
    : m_imageGenerator(buildImageGenerator())
    , m_renderer(buildRenderer(imageWidth, imageHeight, resolution, targetSdf, 
        viewPoint, lookPoint, up, lightPosition, lightIntensity, rgbColor, m_imageGenerator))
{
}

cv::Mat VolplaySdfRenderer::render() const
{
    m_renderer->render();

    return toBgr(m_imageGenerator->image()->toOpenCV());
}

}