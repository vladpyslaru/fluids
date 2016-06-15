#pragma once

#include "GridDefinitions.hpp"

namespace FluidSimulations
{

template <typename ValueType>
inline void setValues(const std::shared_ptr<Grid<ValueType>>& target, const ValueType& value)
{
    for (IntegerType i = 0; i < target->iRes(); ++i)
    {
        for (IntegerType j = 0; j < target->jRes(); ++j)
        {
            for (IntegerType k = 0; k < target->kRes(); ++k)
            {
                target->at(i, j, k) = value;
            }
        }
    }
}

template <typename ValueType>
inline void copyIn(const std::shared_ptr<const Grid<ValueType>>& source, const std::shared_ptr<Grid<ValueType>>& target)
{
    for (IntegerType i = 0; i < target->iRes(); ++i)
    {
        for (IntegerType j = 0; j < target->jRes(); ++j)
        {
            for (IntegerType k = 0; k < target->kRes(); ++k)
            {
                target->at(i, j, k) = source->at(i, j, k);
            }
        }
    }
}

}