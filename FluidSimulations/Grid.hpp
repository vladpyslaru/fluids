#pragma once

#include "ScalarVectorDefinitions.hpp"

#include <memory>

namespace FluidSimulations
{

template <typename ValueType>
class Grid
{
public:
    Grid(const IntegerType iRes, const IntegerType jRes, const IntegerType kRes)
        : m_iRes(iRes)
        , m_jRes(jRes)
        , m_kRes(kRes)
        , m_iMul(jRes * kRes)
        , m_jMul(kRes)
        , m_data(new ValueType[iRes * jRes * kRes])
    {
    }


    Grid(const IntegerType iRes, const IntegerType jRes, const IntegerType kRes, 
        const ValueType& defaultValue)
        : m_iRes(iRes)
        , m_jRes(jRes)
        , m_kRes(kRes)
        , m_iMul(jRes * kRes)
        , m_jMul(kRes)
        , m_data(new ValueType[iRes * jRes * kRes])
    {
        for (ValueType* dataPtr = m_data.get(); dataPtr != m_data.get() + iRes * jRes * kRes;
            *(dataPtr++) = defaultValue);
    }

    Grid(const Grid&) = delete;
    Grid& operator=(const Grid&) = delete;

    inline const IntegerType& iRes() const
    {
        return m_iRes;
    }

    inline const IntegerType& jRes() const
    {
        return m_jRes;
    }

    inline const IntegerType& kRes() const
    {
        return m_kRes;
    }

    inline IntegersThreeDVector res() const
    {
        return IntegersThreeDVector(iRes(), jRes(), kRes());
    }

    inline ValueType& at(const IntegerType i, const IntegerType j, const IntegerType k)
    {
        return m_data[i * m_iMul + j * m_jMul + k];
    }

    inline const ValueType& at(const IntegerType i, const IntegerType j, const IntegerType k) const
    {
        return const_cast<const ValueType&>(const_cast<Grid<ValueType>&>(*this).at(i, j, k));
    }

private:
    const IntegerType m_iRes;
    const IntegerType m_jRes;
    const IntegerType m_kRes;

    const IntegerType m_iMul;
    const IntegerType m_jMul;

    const std::unique_ptr<ValueType[]> m_data;
};

}