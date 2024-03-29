#pragma once

#include "core/types.h"

 


template<typename T>
class FirmVector
{
    static constexpr uint64_t MaxSize = 256; 

public:
    FirmVector()
    {

    }

    uint64_t Size() const { return m_Size; }

    void PushBack(T& value)
    {
        if (m_Size < (m_Capacity - 1))
        {
            m_Data[m_Size++] = value;

            m_BeginPointer = &m_Data[0];
            m_EndPointer = &m_Data[m_Size + 1];
        }
    }

    T& At(uint64_t location)
    {
        if (location < m_Size)
        {
            return m_Data[location];
        }

        return *(T*)(0);
    }

    void Clear()
    {
        m_Size = 0;

        m_BeginPointer = nullptr;
        m_EndPointer = nullptr;
    }

private:
    T m_Data[FirmVector::MaxSize];

    uint64_t m_Capacity = FirmVector::MaxSize;
    uint64_t m_Size = 0;

    T* m_BeginPointer = nullptr;
    T* m_EndPointer = nullptr;

};
