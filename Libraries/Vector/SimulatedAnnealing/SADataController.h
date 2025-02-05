/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#pragma once

#include "SAData.h"
#include "Kernel/SymmetricKernel.h"
#include "Utils/PixelCoords3D.h"
#include "STBNMath.h"

template<typename T, float(*ValueDistance)(T, T)>
class SADataController
{
public:
    SADataController(SAData<T>& data, SymmetricKernel kernelX, SymmetricKernel kernelY, SymmetricKernel kernelZ, float sigmaValue) :
        m_data(data),
        m_kernelX(kernelX),
        m_kernelY(kernelY),
        m_kernelZ(kernelZ),
        m_sigmaValue(sigmaValue),
        m_cachedEnergy(0.0)
    {

    }

    void RecalculateEnergy(size_t pixelIndex)
    {
        const PixelCoords3D pixelCoords = PixelIndexToPixelCoords3D(pixelIndex, m_data.dims);

        const T& centerCell = m_data.cells[pixelIndex];

        m_cachedEnergy -= double(m_data.energy[pixelIndex]);

        m_data.energy[pixelIndex] = 0.0f;

        // X,Y contributions
        {
            size_t imageBegin = PixelCoords3DToPixelIndex({ 0, 0, pixelCoords[2] }, m_data.dims);
            for (int iy = m_kernelY.start(); iy < m_kernelY.end(); ++iy)
            {
                float kernelY = m_kernelY[abs(iy)];
                int pixelY = ((int)pixelCoords[1] + iy + (int)m_data.dims.y) % (int)m_data.dims.y;
                for (int ix = m_kernelX.start(); ix < m_kernelX.end(); ++ix)
                {
                    if (ix == 0 && iy == 0)
                        continue;

                    float kernelX = m_kernelX[abs(ix)];
                    int pixelX = (int)(pixelCoords[0] + ix + (int)m_data.dims.x) % (int)m_data.dims.x;

                    float kernel = kernelX * kernelY;

                    float valueDistance = ValueDistance(m_data.cells[imageBegin + pixelY * m_data.dims.x + pixelX], centerCell);

                    float b = 0.0f;
                    if (T::N == 3)
                        b = valueDistance / (2.0f * m_sigmaValue * m_sigmaValue);
                    else
                        b = powf(valueDistance, float(T::N) / 3.0f) / (2.0f * m_sigmaValue * m_sigmaValue);
                    kernel *= FastExp(-b);

                    m_data.energy[pixelIndex] += kernel;
                }
            }
        }

        // Z contributions
        {
            for (int iz = m_kernelZ.start(); iz < m_kernelZ.end(); ++iz)
            {
                if (iz == 0)
                    continue;

                int pixelZ = ((int)pixelCoords[2] + iz + (int)m_data.dims.z) % (int)m_data.dims.z;

                size_t srcPixelIndex = PixelCoords3DToPixelIndex({ pixelCoords[0], pixelCoords[1], (size_t)pixelZ }, m_data.dims);

                float kernel = m_kernelZ[abs(iz)];

                float valueDistance = ValueDistance(m_data.cells[srcPixelIndex], centerCell);

                float b = 0.0f;
                if (T::N == 3)
                    b = valueDistance / (2.0f * m_sigmaValue * m_sigmaValue);
                else
                    b = powf(valueDistance, float(T::N) / 3.0f) / (2.0f * m_sigmaValue * m_sigmaValue);
                kernel *= FastExp(-b);

                m_data.energy[pixelIndex] += kernel;
            }
        }

        m_cachedEnergy += double(m_data.energy[pixelIndex]);
    }

    float Energy()
    {
        return (float)m_cachedEnergy;
    }

    void SplatOn(size_t index)
    {
        Splat<true>(index);
    }

    void SplatOff(size_t index)
    {
        Splat<false>(index);
    }

    void Swap(size_t indexA, size_t indexB)
    {
        T A = m_data.cells[indexA];
        T B = m_data.cells[indexB];
        float PDFA = m_data.pdf[indexA];
        float PDFB = m_data.pdf[indexB];

        SplatOff(indexA);
        SplatOff(indexB);

        m_data.cells[indexA] = B;
        m_data.cells[indexB] = A;

        m_data.pdf[indexA] = PDFB;
        m_data.pdf[indexB] = PDFA;

        SplatOn(indexA);
        SplatOn(indexB);

        // The energy that other pixels give to indexA and indexB are based
        // on the vector at indexA and indexB so we need to recalculate
        // these energy cells completely.
        RecalculateEnergy(indexA);
        RecalculateEnergy(indexB);
    }

private:
    SAData<T>& m_data;

    SymmetricKernel m_kernelX;
    SymmetricKernel m_kernelY;
    SymmetricKernel m_kernelZ;
    float m_sigmaValue;

    double m_cachedEnergy;

    float CalculateB(float valueDistance)
    {
        float b = 0.0f;
        if (T::N == 3)
            b = valueDistance / (2.0f * m_sigmaValue * m_sigmaValue);
        else
            b = powf(valueDistance, float(T::N) / 3.0f) / (2.0f * m_sigmaValue * m_sigmaValue);
        return b;
    }

    template<bool ON>
    void SplatXY(const PixelCoords3D& pixelCoords, const T centerCell)
    {
        size_t imageBegin = PixelCoords3DToPixelIndex({ 0, 0, pixelCoords[2] }, m_data.dims);
        for (int iy = m_kernelY.start(); iy < m_kernelY.end(); ++iy)
        {
            float kernelY = m_kernelY[abs(iy)];
            int pixelY = ((int)pixelCoords[1] + iy + (int)m_data.dims.y) % (int)m_data.dims.y;
            for (int ix = m_kernelX.start(); ix < m_kernelX.end(); ++ix)
            {
                if (ix == 0 && iy == 0)
                    continue;

                float kernelX = m_kernelX[abs(ix)];
                int pixelX = (int)(pixelCoords[0] + ix + (int)m_data.dims.x) % (int)m_data.dims.x;

                float kernel = kernelX * kernelY;
                if (!ON)
                    kernel *= -1.0f;

                float valueDistance = ValueDistance(m_data.cells[imageBegin + pixelY * m_data.dims.x + pixelX], centerCell);

                float b = CalculateB(valueDistance);

                kernel *= FastExp(-b);

                m_data.energy[imageBegin + pixelY * m_data.dims.x + pixelX] += kernel;

                m_cachedEnergy += double(kernel);
            }
        }
    }

    template<bool ON>
    void SplatZ(const PixelCoords3D& pixelCoords, T centerCell)
    {
        // NOTE! LE not LEQ
        for (int iz = m_kernelZ.start(); iz < m_kernelZ.end(); ++iz)
        {
            if (iz == 0)
                continue;

            int pixelZ = ((int)pixelCoords[2] + iz + (int)m_data.dims.z) % (int)m_data.dims.z;

            size_t pixelIndex = PixelCoords3DToPixelIndex({ pixelCoords[0], pixelCoords[1], (size_t)pixelZ }, m_data.dims);

            float kernel = m_kernelZ[abs(iz)];
            if (!ON)
                kernel *= -1.0f;

            float valueDistance = ValueDistance(m_data.cells[pixelIndex], centerCell);

            float b = CalculateB(valueDistance);

            kernel *= FastExp(-b);

            m_data.energy[pixelIndex] += kernel;

            m_cachedEnergy += double(kernel);
        }
    }

    template<bool ON>
    void Splat(size_t pixelIndex)
    {
        const PixelCoords3D pixelCoords = PixelIndexToPixelCoords3D(pixelIndex, m_data.dims);
        const T& centerCell = m_data.cells[pixelIndex];

        SplatXY<ON>(pixelCoords, centerCell);
        SplatZ<ON>(pixelCoords, centerCell);
    }
};
