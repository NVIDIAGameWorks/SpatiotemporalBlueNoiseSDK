#pragma once

#include <vector>

static const float C_PI = 3.14159265359f;
static const float C_TWOPI = 2.0f*3.14159265359f;

template <typename T>
T Clamp(const T& value, const T& minimum, const T& maximum)
{
    if (value <= minimum)
        return minimum;
    else if (value >= maximum)
        return maximum;
    else
        return value;
}

template <typename T>
T Lerp(const T& A, const T& B, float t)
{
    return A * (1.0f - t) + B * t;
}

template <float c_minimumValue, float c_maximumValue, int c_bucketCount, typename TRET>
struct FunctionLUT1Param
{
    using Fn = TRET(*)(float x);

    FunctionLUT1Param(Fn fn)
    {
        m_LUT.resize(c_bucketCount);

        for (size_t i = 0; i < c_bucketCount; ++i)
        {
            float percent = float(i) / float(c_bucketCount - 1);
            float x = percent * (c_maximumValue - c_minimumValue) + c_minimumValue;
            m_LUT[i] = fn(x);
        }
    }

    TRET Lookup(float x)
    {
        float percent = (x - c_minimumValue) / (c_maximumValue - c_minimumValue);
        int bucketIndex = int(percent * float(c_bucketCount - 1));
        return m_LUT[(size_t)bucketIndex];
    }

    std::vector<TRET> m_LUT;
};

template <float c_minimumValueX, float c_maximumValueX, float c_minimumValueY, float c_maximumValueY, int c_bucketCountX, int c_bucketCountY, typename TRET>
struct FunctionLUT2Param
{
    using Fn = TRET(*)(float x, float y);

    FunctionLUT2Param(Fn fn)
    {
        m_LUT.resize(c_bucketCountX * c_bucketCountY);

        for (int iy = 0; iy < c_bucketCountY; ++iy)
        {
            float percentY = float(iy) / float(c_bucketCountY - 1);
            float y = percentY * (c_maximumValueY - c_minimumValueY) + c_minimumValueY;

            for (int ix = 0; ix < c_bucketCountX; ++ix)
            {
                float percentX = float(ix) / float(c_bucketCountX - 1);
                float x = percentX * (c_maximumValueX - c_minimumValueX) + c_minimumValueX;

                m_LUT[iy * c_bucketCountX + ix] = fn(x, y);
            }
        }
    }

    inline TRET Lookup(float x, float y)
    {
        float percentX = (x - c_minimumValueX) / (c_maximumValueX - c_minimumValueX);
        int ix = int(percentX * float(c_bucketCountX - 1));

        float percentY = (y - c_minimumValueY) / (c_maximumValueY - c_minimumValueY);
        int iy = int(percentY * float(c_bucketCountY - 1));

        return m_LUT[iy * c_bucketCountX + ix];
    }

    std::vector<TRET> m_LUT;
};

static float FastExp(float x)
{
    static FunctionLUT1Param<-5.0f, 0.0f, 100, float> lut([](float x) {return (float)exp(x); });
    return lut.Lookup(x);
}