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

#include <algorithm>

#include "STBNMath.h"
#include "STBNRandom.h"

#include "ImportanceSampling/ImportanceSamplingData.h"
#include "RandomTextureGenerators/PDFStats.h"

template<typename T, T(*RandomTGenerator)(pcg32_random_t& rng)>
class ImportanceSampledRandomTextureGenerator
{
public:
    ImportanceSampledRandomTextureGenerator(const ImportanceSamplingData& isData) :
        m_isData(isData)
    {

    }

    PDFStats Generate(std::vector<T>& cells, std::vector<float>& pdf, pcg32_random_t& rng)
    {
        PDFStats stats;
        for (size_t index = 0; index < cells.size(); ++index)
        {
            while (1)
            {
                cells[index] = RandomTGenerator(rng);
                int isx = Clamp(int((cells[index][0] * 0.5f + 0.5f) * float(m_isData.isw)), 0, m_isData.isw - 1);
                int isy = 0;
                if (T::N > 1)
                {
                    isy = Clamp(int((cells[index][1] * 0.5f + 0.5f) * float(m_isData.ish)), 0, m_isData.ish - 1);
                }

                float isvalue = m_isData.isf[isy * m_isData.isw + isx];
                float randValue = RandomFloat01(rng);
                if (randValue < isvalue)
                {
                    pdf[index] = isvalue / m_isData.isavg;
                    stats.max = std::max(pdf[index], stats.max);
                    stats.min = std::min(pdf[index], stats.min);
                    break;
                }
            }
        }
        return stats;
    }

private:
    ImportanceSamplingData m_isData;
};