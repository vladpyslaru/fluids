#pragma once

#include "../ScalarTypes.hpp"

#include <memory>

namespace FluidSimulations
{

namespace Machinery
{

class ITimeSuggester
{
public:
    virtual ~ITimeSuggester() {}

    virtual FloatType suggest() const = 0;
};

typedef std::shared_ptr<const ITimeSuggester> ITimeSuggesterConstPtr;

}

}