/*
* Copyright (c) 2019-2021, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#pragma once

#include <vector>
#include "../Shared/pcg/pcg_basic.h"

typedef std::array<size_t, 3> PixelCoords;

template <float c_minimumValue, float c_maximumValue, int c_bucketCount, typename TRET>
struct FunctionLUT1Param
{
    using Fn = TRET(*)(float x);

    FunctionLUT1Param(Fn fn)
    {
        m_LUT.resize(c_bucketCount);

        for (int i = 0; i < c_bucketCount; ++i)
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
        return m_LUT[bucketIndex];
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

float FastExp(float x)
{
    static FunctionLUT1Param<-5.0f, 0.0f, 100, float> lut([](float x) {return (float)exp(x); });
    return lut.Lookup(x);
}

template <int N, int dimsX, int dimsY, int dimsZ, float sigmaX, float sigmaY, float sigmaZ, float sigmaValue, bool splatEnergyOverSpace, bool splatEnergyOverTime, bool vectorsCompareWithDotProduct>
struct MakeBase
{
    static const int c_dimension = N;

    static const int c_width = dimsX;
    static const int c_height = dimsY;
    static const int c_depth = dimsZ;

    inline static const std::array<float, 3> c_sigmaDistance = { sigmaX, sigmaY, sigmaZ };
    inline static const float c_sigmaValue = sigmaValue;

    static const int c_numPixels = dimsX * dimsY * dimsZ;

    typedef std::array<float, N> TVec;

    static double constexpr sqrtNewtonRaphson(double x, double curr, double prev)
    {
        return curr == prev
            ? curr
            : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
    }

    static double constexpr sqrtNewtonRaphson(double x)
    {
        return sqrtNewtonRaphson(x, x, 0.0);
    }

    static int constexpr KernelRadius(float sigma)
    {
        float energyLoss = 0.005f;
        float logEnergyLoss = -5.29831736655f;// log(energyLoss);  not a constexpr unfortunately!
        return int(sqrtNewtonRaphson(-2.0f * sigma * sigma * logEnergyLoss));
    }

    template <bool END>
    static int constexpr KernelRadius(float sigma, int dims)
    {
        int radius = KernelRadius(sigma);
        int start = -radius;
        int end = radius;
        int diameter = radius * 2 + 1;

        if (diameter > dims)
        {
            start = -dims / 2;
            end = start + dims;
        }

        if (END)
            return end;
        else
            return start;
    }

    bool CanSwap(uint32_t indexA, uint32_t indexB) { return true; }

    static const int c_KernelRadiusStartX = splatEnergyOverSpace ? KernelRadius<false>(sigmaX, dimsX) : 0;
    static const int c_KernelRadiusEndX = splatEnergyOverSpace ? KernelRadius<true>(sigmaX, dimsX) : 0;
    static const int c_KernelRadiusMaxX = std::max(-c_KernelRadiusStartX, c_KernelRadiusEndX);

    static const int c_KernelRadiusStartY = splatEnergyOverSpace ? KernelRadius<false>(sigmaY, dimsY) : 0;
    static const int c_KernelRadiusEndY = splatEnergyOverSpace ? KernelRadius<true>(sigmaY, dimsY) : 0;
    static const int c_KernelRadiusMaxY = std::max(-c_KernelRadiusStartY, c_KernelRadiusEndY);

    static const int c_KernelRadiusStartZ = splatEnergyOverTime ? KernelRadius<false>(sigmaZ, dimsZ) : 0;
    static const int c_KernelRadiusEndZ = splatEnergyOverTime ? KernelRadius<true>(sigmaZ, dimsZ) : 0;
    static const int c_KernelRadiusMaxZ = std::max(-c_KernelRadiusStartZ, c_KernelRadiusEndZ);

    // member variables

    std::array<float, c_KernelRadiusMaxX + 1> m_kernelX;
    std::array<float, c_KernelRadiusMaxY + 1> m_kernelY;
    std::array<float, c_KernelRadiusMaxZ + 1> m_kernelZ;

    std::vector<TVec> m_cells;
    std::vector<float> m_pdf;
    std::vector<float> m_energy;
    double m_energyTotalCached = 0.0;

    // member functions

    MakeBase()
    {
        // calculate the kernels
        {
            m_kernelX[0] = 1.0f;
            float sum = 1.0f;
            for (size_t index = 1; index <= c_KernelRadiusMaxX; ++index)
            {
                float x = float(index);
                m_kernelX[index] = (float)exp(-(x * x) / (2.0f * sigmaX * sigmaX));
                sum += m_kernelX[index];
            }
            for (float& f : m_kernelX)
                f /= sum;
        }

        {
            m_kernelY[0] = 1.0f;
            float sum = 1.0f;
            for (size_t index = 1; index <= c_KernelRadiusMaxY; ++index)
            {
                float x = float(index);
                m_kernelY[index] = (float)exp(-(x * x) / (2.0f * sigmaY * sigmaY));
                sum += m_kernelY[index];
            }
            for (float& f : m_kernelY)
                f /= sum;
        }

        {
            m_kernelZ[0] = 1.0f;
            float sum = 1.0f;
            for (size_t index = 1; index <= c_KernelRadiusMaxZ; ++index)
            {
                float x = float(index);
                m_kernelZ[index] = (float)exp(-(x * x) / (2.0f * sigmaZ * sigmaZ));
                sum += m_kernelZ[index];
            }
            for (float& f : m_kernelZ)
                f /= sum;
        }

        // allocate
        m_cells.resize(c_numPixels);
        m_pdf.resize(c_numPixels, 1.0f);
        m_energy.resize(c_numPixels, 0.0f);

        m_energyTotalCached = 0.0;
    }

    inline static PixelCoords PixelIndexToPixelCoords(size_t pixelIndex)
    {
        PixelCoords ret;

        ret[0] = (pixelIndex % (dimsX)) / (1);
        ret[1] = (pixelIndex % (dimsX * dimsY)) / (dimsX);
        ret[2] = (pixelIndex % (dimsX * dimsY * dimsZ)) / (dimsX * dimsY);

        return ret;
    }

    inline static size_t PixelCoordsToPixelIndex(const PixelCoords& pixelCoords)
    {
        size_t ret = 0;

        ret += pixelCoords[2];
        ret *= dimsY;
        ret += pixelCoords[1];
        ret *= dimsX;
        ret += pixelCoords[0];

        return ret;
    }

    void RecalculateEnergy(size_t pixelIndex)
    {
        const PixelCoords pixelCoords = PixelIndexToPixelCoords(pixelIndex);

        const TVec& centerCell = m_cells[pixelIndex];

        m_energyTotalCached -= double(m_energy[pixelIndex]);

        m_energy[pixelIndex] = 0.0f;

        // X,Y contributions
        {
            size_t imageBegin = PixelCoordsToPixelIndex({ 0, 0, pixelCoords[2] });
            for (int iy = c_KernelRadiusStartY; iy < c_KernelRadiusEndY; ++iy)
            {
                float kernelY = m_kernelY[abs(iy)];
                int pixelY = ((int)pixelCoords[1] + iy + (int)dimsY) % (int)dimsY;
                for (int ix = c_KernelRadiusStartX; ix < c_KernelRadiusEndX; ++ix)
                {
                    if (ix == 0 && iy == 0)
                        continue;

                    float kernelX = m_kernelX[abs(ix)];
                    int pixelX = (int)(pixelCoords[0] + ix + (int)dimsX) % (int)dimsX;

                    float kernel = kernelX * kernelY;

                    float valueDistance = 0.0f;
                    if (vectorsCompareWithDotProduct)
                        valueDistance = -Dot(m_cells[imageBegin + pixelY * dimsX + pixelX], centerCell);
                    else
                        valueDistance = Length(m_cells[imageBegin + pixelY * dimsX + pixelX] - centerCell);

                    float b = 0.0f;
                    if (N == 3)
                        b = valueDistance / (2.0f * c_sigmaValue * c_sigmaValue);
                    else
                        b = powf(valueDistance, float(N) / 3.0f) / (2.0f * c_sigmaValue * c_sigmaValue);
                    kernel *= FastExp(-b);

                    m_energy[pixelIndex] += kernel;
                }
            }
        }

        // Z contributions
        {
            for (int iz = c_KernelRadiusStartZ; iz < c_KernelRadiusEndZ; ++iz)
            {
                if (iz == 0)
                    continue;

                int pixelZ = ((int)pixelCoords[2] + iz + (int)dimsZ) % (int)dimsZ;

                size_t srcPixelIndex = PixelCoordsToPixelIndex({ pixelCoords[0], pixelCoords[1], (size_t)pixelZ });

                float kernel = m_kernelZ[abs(iz)];

                float valueDistance = 0.0f;
                if (vectorsCompareWithDotProduct)
                    valueDistance = -Dot(m_cells[srcPixelIndex], centerCell);
                else
                    valueDistance = Length(m_cells[srcPixelIndex] - centerCell);

                float b = 0.0f;
                if (N == 3)
                    b = valueDistance / (2.0f * c_sigmaValue * c_sigmaValue);
                else
                    b = powf(valueDistance, float(N) / 3.0f) / (2.0f * c_sigmaValue * c_sigmaValue);
                kernel *= FastExp(-b);

                m_energy[pixelIndex] += kernel;
            }
        }

        m_energyTotalCached += double(m_energy[pixelIndex]);
    }

    template <bool ON>
    void SplatEnergy(size_t pixelIndex)
    {
        const PixelCoords pixelCoords = PixelIndexToPixelCoords(pixelIndex);

        const TVec& centerCell = m_cells[pixelIndex];

        // splat the 2D gaussian across XY
        {
            size_t imageBegin = PixelCoordsToPixelIndex({ 0, 0, pixelCoords[2] });
            for (int iy = c_KernelRadiusStartY; iy < c_KernelRadiusEndY; ++iy)
            {
                float kernelY = m_kernelY[abs(iy)];
                int pixelY = ((int)pixelCoords[1] + iy + (int)dimsY) % (int)dimsY;
                for (int ix = c_KernelRadiusStartX; ix < c_KernelRadiusEndX; ++ix)
                {
                    if (ix == 0 && iy == 0)
                        continue;

                    float kernelX = m_kernelX[abs(ix)];
                    int pixelX = (int)(pixelCoords[0] + ix + (int)dimsX) % (int)dimsX;

                    float kernel = kernelX * kernelY;
                    if (!ON)
                        kernel *= -1.0f;

                    //float valueDistance = DistanceTorroidalUnitSquared(m_cells[imageBegin + pixelY * dimsX + pixelX], centerCell);
                    //valueDistance = sqrtf(valueDistance);

                    float valueDistance = 0.0f;
                    if (vectorsCompareWithDotProduct)
                        valueDistance = -Dot(m_cells[imageBegin + pixelY * dimsX + pixelX], centerCell);
                    else
                        valueDistance = Length(m_cells[imageBegin + pixelY * dimsX + pixelX] - centerCell);

                    float b = 0.0f;
                    if(N == 3)
                        b = valueDistance / (2.0f * c_sigmaValue * c_sigmaValue);
                    else
                        b = powf(valueDistance, float(N) / 3.0f) / (2.0f * c_sigmaValue * c_sigmaValue);
                    kernel *= FastExp(-b);

                    m_energy[imageBegin + pixelY * dimsX + pixelX] += kernel;

                    m_energyTotalCached += double(kernel);
                }
            }
        }

        // splat the 1D gaussian along z
        {
            for (int iz = c_KernelRadiusStartZ; iz < c_KernelRadiusEndZ; ++iz)
            {
                if (iz == 0)
                    continue;

                int pixelZ = ((int)pixelCoords[2] + iz + (int)dimsZ) % (int)dimsZ;

                size_t pixelIndex = PixelCoordsToPixelIndex({ pixelCoords[0], pixelCoords[1], (size_t)pixelZ });

                float kernel = m_kernelZ[abs(iz)];
                if (!ON)
                    kernel *= -1.0f;

                //float valueDistance = DistanceTorroidalUnitSquared(m_cells[pixelIndex], centerCell);
                //valueDistance = sqrtf(valueDistance);

                float valueDistance = 0.0f;
                if (vectorsCompareWithDotProduct)
                    valueDistance = -Dot(m_cells[pixelIndex], centerCell);
                else
                    valueDistance = Length(m_cells[pixelIndex] - centerCell);

                float b = 0.0f;
                if (N == 3)
                    b = valueDistance / (2.0f * c_sigmaValue * c_sigmaValue);
                else
                    b = powf(valueDistance, float(N) / 3.0f) / (2.0f * c_sigmaValue * c_sigmaValue);

                kernel *= FastExp(-b);

                m_energy[pixelIndex] += kernel;

                m_energyTotalCached += double(kernel);
            }
        }
    }

    template <typename TMAKER>
    static void GenerateStartingTexture(TMAKER& maker, pcg32_random_t& rng, bool importanceSampling, std::vector<float>& isf, int isw, int ish, float isavg, const char* baseFileName)
    {
        if (importanceSampling)
        {
            for (size_t index = 0; index < TMAKER::c_numPixels; ++index)
            {
                while (1)
                {
                    TMAKER::Generate(maker.m_cells[index], rng);
                    int isx = Clamp(int((maker.m_cells[index] * 0.5f + 0.5f)[0] * float(isw)), 0, isw - 1);
                    int isy = 0;
                    if (maker.m_cells[0].size() > 1)  // if the importance map is 2 dimensional
                    {
                        isy = Clamp(int((maker.m_cells[index] * 0.5f + 0.5f)[1] * float(ish)), 0, ish - 1);
                    }

                    float isvalue = isf[isy * isw + isx];
                    float randValue = RandomFloat01(rng);
                    if (randValue < isvalue)
                    {
                        maker.m_pdf[index] = isvalue / isavg;
                        break;
                    }
                }
            }
        }
        else
        {
            for (size_t index = 0; index < c_numPixels; ++index)
            {
                TMAKER::Generate(maker.m_cells[index], rng);
                maker.m_pdf[index] = 1.0f;
            }
        }
    }

    float Energy() const
    {
        return (float)m_energyTotalCached;
    }
};
